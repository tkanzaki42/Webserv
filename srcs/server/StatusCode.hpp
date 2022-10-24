#ifndef SRCS_SERVER_STATUSCODE_HPP_
#define SRCS_SERVER_STATUSCODE_HPP_

class StatusCode {
 public:
    static const int HTTP_CONTINUE                        = 100;
    static const int HTTP_SWITCHING_PROTOCOLS             = 101;
    static const int HTTP_PROCESSING                      = 102;
    static const int HTTP_OK                              = 200;
    static const int HTTP_CREATED                         = 201;
    static const int HTTP_ACCEPTED                        = 202;
    static const int HTTP_NON_AUTHORITATIVE               = 203;
    static const int HTTP_NO_CONTENT                      = 204;
    static const int HTTP_RESET_CONTENT                   = 205;
    static const int HTTP_PARTIAL_CONTENT                 = 206;
    static const int HTTP_MULTI_STATUS                    = 207;
    static const int HTTP_ALREADY_REPORTED                = 208;
    static const int HTTP_IM_USED                         = 226;
    static const int HTTP_MULTIPLE_CHOICES                = 300;
    static const int HTTP_MOVED_PERMANENTLY               = 301;
    static const int HTTP_MOVED_TEMPORARILY               = 302;
    static const int HTTP_SEE_OTHER                       = 303;
    static const int HTTP_NOT_MODIFIED                    = 304;
    static const int HTTP_USE_PROXY                       = 305;
    static const int HTTP_TEMPORARY_REDIRECT              = 307;
    static const int HTTP_PERMANENT_REDIRECT              = 308;
    static const int HTTP_BAD_REQUEST                     = 400;
    static const int HTTP_UNAUTHORIZED                    = 401;
    static const int HTTP_PAYMENT_REQUIRED                = 402;
    static const int HTTP_FORBIDDEN                       = 403;
    static const int HTTP_NOT_FOUND                       = 404;
    static const int HTTP_METHOD_NOT_ALLOWED              = 405;
    static const int HTTP_NOT_ACCEPTABLE                  = 406;
    static const int HTTP_PROXY_AUTHENTICATION_REQUIRED   = 407;
    static const int HTTP_REQUEST_TIME_OUT                = 408;
    static const int HTTP_CONFLICT                        = 409;
    static const int HTTP_GONE                            = 410;
    static const int HTTP_LENGTH_REQUIRED                 = 411;
    static const int HTTP_PRECONDITION_FAILED             = 412;
    static const int HTTP_REQUEST_ENTITY_TOO_LARGE        = 413;
    static const int HTTP_REQUEST_URI_TOO_LARGE           = 414;
    static const int HTTP_UNSUPPORTED_MEDIA_TYPE          = 415;
    static const int HTTP_RANGE_NOT_SATISFIABLE           = 416;
    static const int HTTP_EXPECTATION_FAILED              = 417;
    static const int HTTP_IM_A_TEAPOT                     = 418;
    static const int HTTP_MISDIRECTED_REQUEST             = 421;
    static const int HTTP_UNPROCESSABLE_ENTITY            = 422;
    static const int HTTP_LOCKED                          = 423;
    static const int HTTP_FAILED_DEPENDENCY               = 424;
    static const int HTTP_TOO_EARLY                       = 425;
    static const int HTTP_UPGRADE_REQUIRED                = 426;
    static const int HTTP_PRECONDITION_REQUIRED           = 428;
    static const int HTTP_TOO_MANY_REQUESTS               = 429;
    static const int HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
    static const int HTTP_UNAVAILABLE_FOR_LEGAL_REASONS   = 451;
    static const int HTTP_INTERNAL_SERVER_ERROR           = 500;
    static const int HTTP_NOT_IMPLEMENTED                 = 501;
    static const int HTTP_BAD_GATEWAY                     = 502;
    static const int HTTP_SERVICE_UNAVAILABLE             = 503;
    static const int HTTP_GATEWAY_TIME_OUT                = 504;
    static const int HTTP_VERSION_NOT_SUPPORTED           = 505;
    static const int HTTP_VARIANT_ALSO_VARIES             = 506;
    static const int HTTP_INSUFFICIENT_STORAGE            = 507;
    static const int HTTP_LOOP_DETECTED                   = 508;
    static const int HTTP_NOT_EXTENDED                    = 510;
    static const int HTTP_NETWORK_AUTHENTICATION_REQUIRED = 511;
};

#endif  // SRCS_SERVER_STATUSCODE_HPP_
