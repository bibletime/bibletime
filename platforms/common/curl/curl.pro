# Curl is optional package (http://curl.haxx.se/download/curl-7.34.0.tar.gz)
# Make sure variable CURL_PATH and CURL_LIB points to correct location
# This library must be built separately

isEmpty(CURL_PATH):CURL_PATH = ../../../../curl/curl-7.33.0

INCLUDEPATH += $${CURL_PATH}/include

DEFINES += CURL_STATICLIB


DEFINES += CURLAVAILABLE CURLSFTPAVAILABLE

DEFINES += CURL_DISABLE_TFTP
#DEFINES += CURL_DISABLE_FTP
DEFINES += CURL_DISABLE_LDAP
DEFINES += CURL_DISABLE_TELNET
DEFINES += CURL_DISABLE_DICT
DEFINES += CURL_DISABLE_FILE
DEFINES += CURL_DISABLE_POP3
DEFINES += CURL_DISABLE_IMAP
DEFINES += CURL_DISABLE_SMTP
DEFINES += CURL_DISABLE_RTSP
#DEFINES += CURL_DISABLE_RTMP
DEFINES += CURL_DISABLE_GOPHER


SOURCES += \
    $${CURL_PATH}/lib/easy.c \
    $${CURL_PATH}/lib/timeval.c \
    $${CURL_PATH}/lib/base64.c \
    $${CURL_PATH}/lib/hostip.c \
    $${CURL_PATH}/lib/progress.c \
    $${CURL_PATH}/lib/formdata.c \
    $${CURL_PATH}/lib/cookie.c \
    $${CURL_PATH}/lib/http.c \
    $${CURL_PATH}/lib/sendf.c \
    $${CURL_PATH}/lib/ftp.c \
    $${CURL_PATH}/lib/if2ip.c \
    $${CURL_PATH}/lib/speedcheck.c \
    $${CURL_PATH}/lib/ssluse.c \
    $${CURL_PATH}/lib/version.c \
    $${CURL_PATH}/lib/getenv.c \
    $${CURL_PATH}/lib/escape.c \
    $${CURL_PATH}/lib/mprintf.c \
    $${CURL_PATH}/lib/netrc.c \
    $${CURL_PATH}/lib/getinfo.c \
    $${CURL_PATH}/lib/transfer.c \
    $${CURL_PATH}/lib/strequal.c \
    $${CURL_PATH}/lib/security.c \
    $${CURL_PATH}/lib/curl_fnmatch.c \
    $${CURL_PATH}/lib/fileinfo.c \
    $${CURL_PATH}/lib/ftplistparser.c \
    $${CURL_PATH}/lib/wildcard.c \
    $${CURL_PATH}/lib/krb5.c \
    $${CURL_PATH}/lib/memdebug.c \
    $${CURL_PATH}/lib/http_chunks.c \
    $${CURL_PATH}/lib/strtok.c \
    $${CURL_PATH}/lib/connect.c \
    $${CURL_PATH}/lib/llist.c \
    $${CURL_PATH}/lib/hash.c \
    $${CURL_PATH}/lib/multi.c \
    $${CURL_PATH}/lib/content_encoding.c \
    $${CURL_PATH}/lib/share.c \
    $${CURL_PATH}/lib/http_digest.c \
    $${CURL_PATH}/lib/md4.c \
    $${CURL_PATH}/lib/md5.c \
    $${CURL_PATH}/lib/http_negotiate.c \
    $${CURL_PATH}/lib/inet_pton.c \
    $${CURL_PATH}/lib/strtoofft.c \
    $${CURL_PATH}/lib/strerror.c \
    $${CURL_PATH}/lib/amigaos.c \
    $${CURL_PATH}/lib/hostasyn.c \
    $${CURL_PATH}/lib/hostip4.c \
    $${CURL_PATH}/lib/hostip6.c \
    $${CURL_PATH}/lib/hostsyn.c \
    $${CURL_PATH}/lib/inet_ntop.c \
    $${CURL_PATH}/lib/parsedate.c \
    $${CURL_PATH}/lib/select.c \
    $${CURL_PATH}/lib/gtls.c \
    $${CURL_PATH}/lib/sslgen.c \
    $${CURL_PATH}/lib/splay.c \
    $${CURL_PATH}/lib/strdup.c \
    $${CURL_PATH}/lib/socks.c \
    $${CURL_PATH}/lib/ssh.c \
    $${CURL_PATH}/lib/nss.c \
    $${CURL_PATH}/lib/qssl.c \
    $${CURL_PATH}/lib/curl_addrinfo.c \
    $${CURL_PATH}/lib/socks_gssapi.c \
    $${CURL_PATH}/lib/socks_sspi.c \
    $${CURL_PATH}/lib/curl_sspi.c \
    $${CURL_PATH}/lib/slist.c \
    $${CURL_PATH}/lib/nonblock.c \
    $${CURL_PATH}/lib/curl_memrchr.c \
    $${CURL_PATH}/lib/pingpong.c \
    $${CURL_PATH}/lib/curl_threads.c \
    $${CURL_PATH}/lib/warnless.c \
    $${CURL_PATH}/lib/hmac.c \
    $${CURL_PATH}/lib/polarssl.c \
    $${CURL_PATH}/lib/polarssl_threadlock.c \
    $${CURL_PATH}/lib/curl_rtmp.c \
    #$${CURL_PATH}/lib/openldap.c \
    $${CURL_PATH}/lib/curl_gethostname.c \
    $${CURL_PATH}/lib/axtls.c \
    $${CURL_PATH}/lib/idn_win32.c \
    $${CURL_PATH}/lib/http_negotiate_sspi.c \
    $${CURL_PATH}/lib/cyassl.c \
    $${CURL_PATH}/lib/http_proxy.c \
    $${CURL_PATH}/lib/non-ascii.c \
    $${CURL_PATH}/lib/asyn-ares.c \
    $${CURL_PATH}/lib/asyn-thread.c \
    $${CURL_PATH}/lib/curl_gssapi.c \
    $${CURL_PATH}/lib/curl_ntlm.c \
    $${CURL_PATH}/lib/curl_ntlm_wb.c \
    $${CURL_PATH}/lib/curl_ntlm_core.c \
    $${CURL_PATH}/lib/curl_ntlm_msgs.c \
    $${CURL_PATH}/lib/curl_sasl.c \
    $${CURL_PATH}/lib/curl_schannel.c \
    $${CURL_PATH}/lib/curl_multibyte.c \
    $${CURL_PATH}/lib/curl_darwinssl.c \
    $${CURL_PATH}/lib/hostcheck.c \
    $${CURL_PATH}/lib/bundles.c \
    $${CURL_PATH}/lib/conncache.c \
    $${CURL_PATH}/lib/pipeline.c \
    $${CURL_PATH}/lib/dotdot.c \
    $${CURL_PATH}/lib/x509asn1.c \
    $${CURL_PATH}/lib/gskit.c \
    $${CURL_PATH}/lib/http2.c \
    #$${CURL_PATH}/lib/libcurl.rc \
    $${CURL_PATH}/lib/curl_rawstr.c \
    $${CURL_PATH}/lib/curl_url.c \

!contains(DEFINES, CURL_DISABLE_FILE):SOURCES += $${CURL_PATH}/lib/file.c
!contains(DEFINES, CURL_DISABLE_TFTP):SOURCES += $${CURL_PATH}/lib/tftp.c
!contains(DEFINES, CURL_DISABLE_TELNET):SOURCES += $${CURL_PATH}/lib/telnet.c
!contains(DEFINES, CURL_DISABLE_DICT):SOURCES += $${CURL_PATH}/lib/dict.c
!contains(DEFINES, CURL_DISABLE_POP3):SOURCES += $${CURL_PATH}/lib/pop3.c
!contains(DEFINES, CURL_DISABLE_IMAP):SOURCES += $${CURL_PATH}/lib/imap.c
!contains(DEFINES, CURL_DISABLE_SMTP):SOURCES += $${CURL_PATH}/lib/smtp.c
!contains(DEFINES, CURL_DISABLE_RTSP):SOURCES += $${CURL_PATH}/lib/rtsp.c
!contains(DEFINES, CURL_DISABLE_GOPHER):SOURCES += $${CURL_PATH}/lib/gopher.c

# do we really need CRYPTO_AUTH?
DEFINES += CURL_DISABLE_CRYPTO_AUTH
