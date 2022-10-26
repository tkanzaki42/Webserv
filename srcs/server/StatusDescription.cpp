#include "srcs/server/StatusDescription.hpp"
#include "srcs/util/StringConverter.hpp"
#include "srcs/server/StatusCode.hpp"

// https://httpwg.org/specs/rfc9110.html#overview.of.status.codes
// https://github.com/apache/httpd/blob/trunk/modules/http/http_protocol.c
// の get_canned_error_string
// を参考
std::string StatusDescription::get_message(int status_code) {
    switch (status_code) {
      // 正常
      case StatusCode::HTTP_CREATED:
          return std::string("Successfully uploaded the file.");
      // エラー
      case StatusCode::HTTP_PROXY_AUTHENTICATION_REQUIRED:
      case StatusCode::HTTP_UNAUTHORIZED:
          return std::string("This server could not verify that you are authorized to access the URL.");
      case StatusCode::HTTP_BAD_REQUEST:
          return std::string("Your browser (or proxy) sent a request that this server could not understand.");
      case StatusCode::HTTP_FORBIDDEN:
          return std::string("You don't have permission to access the requested object. It is either read-protected or not readable by the server.");
      case StatusCode::HTTP_NOT_FOUND:
          return std::string("The requested URL was not found on this server.");
      case StatusCode::HTTP_METHOD_NOT_ALLOWED:
          return std::string("The method is not allowed for the requested URL.");
      case StatusCode::HTTP_NOT_ACCEPTABLE:
          return std::string("An appropriate representation of the requested resource could not be found on this server.");
      case StatusCode::HTTP_LENGTH_REQUIRED:
          return std::string("A request of the requested method requires a valid Content-length.");
      case StatusCode::HTTP_PRECONDITION_FAILED:
          return std::string("The precondition on the request for this URL evaluated to false.");
      case StatusCode::HTTP_NOT_IMPLEMENTED:
          return std::string("This page is not supported for current URL.");
      case StatusCode::HTTP_BAD_GATEWAY:
          return std::string("The proxy server received an invalid response from an upstream server.");
      case StatusCode::HTTP_VARIANT_ALSO_VARIES:
          return std::string("A variant for the requested resource is itself a negotiable resource. This indicates a configuration error.");
      case StatusCode::HTTP_REQUEST_TIME_OUT:
          return std::string("Server timeout waiting for the HTTP request from the client.");
      case StatusCode::HTTP_GONE:
          return std::string("The requested resource is no longer available on this server and there is no forwarding address.Please remove all references to this resource.");
      case StatusCode::HTTP_REQUEST_ENTITY_TOO_LARGE:
          return std::string("The requested URL's length exceeds the capacity limit for this server.");
      case StatusCode::HTTP_UNSUPPORTED_MEDIA_TYPE:
          return std::string("The supplied request data is not in a format acceptable for processing by this resource.");
      case StatusCode::HTTP_RANGE_NOT_SATISFIABLE:
          return std::string("None of the range-specifier values in the Range request-header field overlap the current extent of the selected resource.");
      case StatusCode::HTTP_EXPECTATION_FAILED:
          return std::string("Only the 100-continue expectation is supported.");
      case StatusCode::HTTP_UNPROCESSABLE_ENTITY:
          return std::string("The server understands the media type of the request entity, but was unable to process the contained instructions.");
      case StatusCode::HTTP_LOCKED:
          return std::string("The requested resource is currently locked. The lock must be released or proper identification given before the method can be applied. ");
      case StatusCode::HTTP_FAILED_DEPENDENCY:
          return std::string("The method could not be performed on the resource because the requested action depended on another action and that other action failed.");
      case StatusCode::HTTP_TOO_EARLY:
          return std::string("The request could not be processed as TLS early data and should be retried.");
      case StatusCode::HTTP_UPGRADE_REQUIRED:
          return std::string("The requested resource can only be retrieved using SSL.  The server is willing to upgrade the current connection to SSL, but your client doesn't support it. Either upgrade your client, or try requesting the page using https://");
      case StatusCode::HTTP_PRECONDITION_REQUIRED:
          return std::string("The request is required to be conditional.");
      case StatusCode::HTTP_TOO_MANY_REQUESTS:
          return std::string("The user has sent too many requests in a given amount of time.");
      case StatusCode::HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE:
          return std::string("The server refused this request because the request header fields are too large.");
      case StatusCode::HTTP_INSUFFICIENT_STORAGE:
          return std::string("The method could not be performed on the resource because the server is unable to store the representation needed to successfully complete the request.  There is insufficient free space left in your storage allocation.");
      case StatusCode::HTTP_SERVICE_UNAVAILABLE:
          return std::string("The server is temporarily unable to service your request due to maintenance downtime or capacity problems. Please try again later.");
      case StatusCode::HTTP_GATEWAY_TIME_OUT:
          return std::string("The gateway did not receive a timely response from the upstream server or application.");
      case StatusCode::HTTP_LOOP_DETECTED:
          return std::string("The server terminated an operation because it encountered an infinite loop.");
      case StatusCode::HTTP_NOT_EXTENDED:
          return std::string("A mandatory extension policy in the request is not accepted by the server for this resource.");
      case StatusCode::HTTP_NETWORK_AUTHENTICATION_REQUIRED:
          return std::string("The client needs to authenticate to gain network access.");
      case StatusCode::HTTP_IM_A_TEAPOT:
          return std::string("The resulting entity body MAY be short and stout.");
      case StatusCode::HTTP_MISDIRECTED_REQUEST:
          return std::string("The client needs a new connection for this request as the requested host name does not match the Server Name Indication (SNI) in use for this connection.");
      case StatusCode::HTTP_UNAVAILABLE_FOR_LEGAL_REASONS:
          return std::string("Access to this URL has been denied for legal reasons.");
      default:
          return std::string("This page isn't working. HTTP ERROR ") + StringConverter::itos(status_code);
    }
}
