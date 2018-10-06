#include "mailer.h"
#include <curl/curl.h>
#include <set>
#include <sstream>
#include <random>

Mailer& Mailer::setServer(const std::string value) {
    server = value;
    return *this;
}

Mailer& Mailer::setPort(int value) {
    port = value;
    return *this;
}

Mailer& Mailer::setUsername(const std::string value) {
    username = value;
    return *this;
}

Mailer& Mailer::setPassword(const std::string value) {
    password = value;
    return *this;
}

Mailer& Mailer::setFrom(const std::string value) {
    from = value;
    return *this;
}

Mailer& Mailer::setTo(const std::set<std::string> value) {
    to = value;
    return *this;
}

Mailer& Mailer::setCc(const std::set<std::string> value) {
    cc = value;
    return *this;
}

Mailer& Mailer::setBcc(const std::set<std::string> value) {
    bcc = value;
    return *this;
}

Mailer& Mailer::setVerbose(const bool value) {
    verbose = value;
    return *this;
}

Mailer& Mailer::setTls(const bool value) {
    tls = value;
    return *this;
}

Mailer& Mailer::setBody(const std::string value) {
    body = value;
    return *this;
}

Mailer& Mailer::setSubject(const std::string value) {
    subject = value;
    return *this;
}

std::string Mailer::dateHeader(std::string date) {
    std::stringstream result;
    result << "Date: " << date;
    return result.str();
}

std::string Mailer::toHeader() {
    std::stringstream result;
    result << "To: ";

    for (auto item : to) {
        result << item << ",";
    }

    return result.str();
}

std::string Mailer::fromHeader() {
    std::stringstream result;
    result << "From: " << from;
    return result.str();
}

std::string Mailer::ccHeader() {
    std::stringstream result;
    result << "Cc: ";

    for (auto item : cc) {
        result << item << ",";
    }

    return result.str();
}

std::string Mailer::messageIdHeader(long random, std::string domain) {
    std::stringstream result;
    result << "Message-ID: <" << std::hex << random << domain << ">";
    return result.str();
}

std::string Mailer::subjectHeader() {
    std::stringstream result;
    result << "Subject: " << subject;
    return result.str();
}

std::string Mailer::getDomain() {
    return from.substr(from.find('@'), from.length());
}

long Mailer::generateRandom() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<long> dis(std::llround(std::pow(2,61)), std::llround(std::pow(2,62)));
    return dis(gen);
}

std::string Mailer::dateTimeNow() {
    const int RFC5322_TIME_LEN = 31;
    time_t t;
    struct tm *tm;

    std::string ret;
    ret.resize(RFC5322_TIME_LEN);

    time(&t);
    tm = localtime(&t);

    std::strftime(&ret[0], RFC5322_TIME_LEN, "%a, %d %b %Y %H:%M:%S %z", tm);

    return ret;
}


void Mailer::deliver() {
    if (server.empty() || from.empty() || to.empty() || subject.empty() || body.empty()) {
        throw std::invalid_argument("Make sure you set: server, from, to, subject, body!");
    }

    headers = {
        fromHeader(),
        toHeader(),
        dateHeader(dateTimeNow()),
        ccHeader(),
        messageIdHeader(generateRandom(), getDomain()),
        subjectHeader()
    };

    curl = curl_easy_init();

    if (curl == NULL) {
        cleanup();
        throw std::runtime_error("Failed to initialize libcurl");
    }

    setOptions();
    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        cleanup();
        throw std::runtime_error(curl_easy_strerror(res));
    }
}

void Mailer::setOptions() {
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    curl_easy_setopt(curl, CURLOPT_PORT, port);
    curl_easy_setopt(curl, CURLOPT_URL, server.c_str());
    if (tls) {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    }
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());

    for (const auto item : to) {
        recipients = curl_slist_append(recipients, item.c_str());
    }

    for (const auto item : cc) {
        recipients = curl_slist_append(recipients, item.c_str());
    }

    for (const auto item : bcc) {
        recipients = curl_slist_append(recipients, item.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    for (const auto item: headers) {
        curlHeaders = curl_slist_append(curlHeaders, item.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);

    mime = curl_mime_init(curl);
    alt = curl_mime_init(curl);

    part = curl_mime_addpart(alt);
    curl_mime_data(part, body.c_str(), CURL_ZERO_TERMINATED);
    curl_mime_type(part, "text/plain");

    /* Create the inline part. */ 
    part = curl_mime_addpart(mime);
    curl_mime_subparts(part, alt);
    curl_mime_type(part, "multipart/alternative");
    slist = curl_slist_append(nullptr, "Content-Disposition: inline");
    curl_mime_headers(part, slist, 1);

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
}

void Mailer::cleanup() {
    curl_slist_free_all(recipients);
    curl_slist_free_all(curlHeaders);
    curl_easy_cleanup(curl);
    curl_mime_free(mime);
}