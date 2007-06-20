#include <netembryo/rtsp_errors.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

RTSP_Error const RTSP_Ok = { {200, "OK"}, FALSE };
RTSP_Error const RTSP_BadRequest = { {400, "Bad Request"}, TRUE };
RTSP_Error const RTSP_Forbidden = { {403, "Forbidden"}, TRUE };
RTSP_Error const RTSP_NotFound = { {404, "Not Found"}, TRUE };
RTSP_Error const RTSP_SessionNotFound = { {454, "Session Not Found"}, TRUE };
RTSP_Error const RTSP_InternalServerError = { {500, "Internal Server Error"}, TRUE };
RTSP_Error const RTSP_OptionNotSupported = { {551, "Option not supported"}, TRUE };

/**
 * sets an RTSP_Error to a specific error
 * @param err the pointer to the error variable to edit
 * @param reply_code the code of RTSP reply message
 * @param message the content of the RTSP reply message
 */
void set_RTSP_Error(RTSP_Error * err, int reply_code, char * message)
{
    err->got_error = TRUE;
    err->message.reply_code = reply_code;
    strncpy(err->message.reply_str, message, RTSP_MAX_REPLY_MESSAGE_LEN);
}

/**
 * Retrieves the RTSP_Error of the given standard reply code
 * @param reply_code The code of the RTSP reply message
 * @return A pointer to an RTSP_Error structure for the given standard code or NULL
 */
RTSP_Error const * get_RTSP_Error(int reply_code)
{
    switch (reply_code)
    {
        case 200:
            return &RTSP_Ok;
        case 400:
            return &RTSP_BadRequest;
        case 403:
            return &RTSP_Forbidden;
        case 404:
            return &RTSP_NotFound;
        case 454:
            return &RTSP_SessionNotFound;
        case 500:
            return &RTSP_InternalServerError;
        case 551:
            return &RTSP_OptionNotSupported;
        default:
            return NULL;
    }
}
