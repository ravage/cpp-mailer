#ifndef MAILER_H
#define MAILER_H

#include <string>
#include <set>
#include <curl/curl.h>

class Mailer {
    private:

    std::string server;
    int port{25};
    std::string username;
    std::string password;
    std::string from;
    std::string subject;
    std::string body;
    std::set<std::string> to;
    std::set<std::string> cc;
    std::set<std::string> bcc;
    std::set<std::string> headers;
    bool verbose;
    bool tls;

    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients{nullptr};
    struct curl_slist *curlHeaders{nullptr};
    struct curl_slist *slist{nullptr};
    curl_mime *mime{nullptr};
    curl_mime *alt{nullptr};
    curl_mimepart *part{nullptr};

    std::string dateHeader(const std::string&);
    std::string toHeader();
    std::string fromHeader();
    std::string ccHeader();
    std::string messageIdHeader(long, const std::string&);
    std::string subjectHeader();
    std::string getDomain();
    void setOptions();
    long generateRandom();
    std::string dateTimeNow();
    void cleanup();

    public:

    Mailer& setServer(const std::string&);
    Mailer& setPort(const int);
    Mailer& setUsername(const std::string&);
    Mailer& setPassword(const std::string&);
    Mailer& setFrom(const std::string&);
    Mailer& setTo(const std::set<std::string>&);
    Mailer& setCc(const std::set<std::string>&);
    Mailer& setBcc(const std::set<std::string>&);
    Mailer& setVerbose(const bool value = true);
    Mailer& setTls(const bool value = true);
    Mailer& setBody(const std::string&);
    Mailer& setSubject(const std::string&);
    void deliver();
};

#endif