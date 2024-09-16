#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
    // Configuring syslog
    openlog("writer", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Starting logging for writer...");

    // Check for correct number of arguments
    if (argc != 3)
    {
        syslog(LOG_ERR, "Error: Two arguments required: <writefile> <writestr>");
        closelog();
        return 1;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    // Open the file for writing
    FILE *file = fopen(writefile, "w");
    if (file == NULL)
    {
        syslog(LOG_ERR, "Error: Could not create or write to file %s: %s", writefile, strerror(errno));
        closelog();
        return 1;
    }

    // Write the string to the file
    if (fprintf(file, "%s\n", writestr) < 0)
    {
        syslog(LOG_ERR, "Error: Could not write to file %s: %s", writefile, strerror(errno));
        fclose(file);
        closelog();
        return 1;
    }

    // Close the file
    if (fclose(file) != 0)
    {
        syslog(LOG_ERR, "Error: Could not close file %s: %s", writefile, strerror(errno));
        closelog();
        return 1;
    }

    syslog(LOG_INFO, "End of logging for writer...");
    closelog();
    return 0;
}
