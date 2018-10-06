#include "mailer.h"

int main() {
    Mailer mailer;

    mailer.setServer("smtp://smtp.mailtrap.io")
        .setUsername(std::getenv("USERNAME"))
        .setPassword(std::getenv("PASSWORD"))
        .setFrom("user@me.com")
        .setTo({"<user@example.com>", "<user@example.net>"})
        .setVerbose()
        .setTls()
        .setSubject("Subject Text!")
        .setBody("Body Text");
    mailer.deliver();

    return 0;
}
