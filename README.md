README for BibleTime
====================

BibleTime is a Bible study application based on the Sword library and
Qt toolkit.

## INSTALLATION

You can just download and install BibleTime from
[GitHub](https://github.com/bibletime/bibletime/releases/latest).
The following requirements are needed only if you want to develop BibleTime.

### BUILD REQUIREMENTS
 - A C++17 compiler.
 - [Qt 6.7+](https://www.qt.io)
 - [Sword 1.8.1+](https://crosswire.org/sword) (built against ICU)
 - [CLucene 2.3.3.4+](https://clucene.sf.net)
 - [CMake 3.25+](https://cmake.org)
 - [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) (or
   [pkgconf](https://gitea.treehouse.systems/ariadne/pkgconf))

### DOCUMENTATION GENERATION REQUIREMENTS (OPTIONAL)
 - [docbook-xml](https://docbook.org/)
 - [docbook-xsl](https://github.com/docbook/wiki/wiki/DocBookXslStylesheets)
 - [fop](https://xmlgraphics.apache.org/fop/) (for PDF generation)
 - [po4a](https://po4a.org/)
 - [xsltproc](https://gitlab.gnome.org/GNOME/libxslt)

These packages are only used by the build system when document generation and
installation is enabled. By default, all translations of the handbook and howto
documents are generated and installed. This can be changed using the following
options to CMake:

 * `BUILD_HANDBOOK_HTML` - whether to generate and install the HTML handbook
   (default: "ON")
 * `BUILD_HANDBOOK_HTML_LANGUAGES`
       - list of language codes to use for HTML handbook, or empty for all
         languages (default: "")
 * `BUILD_HANDBOOK_PDF` - whether to generate and install the PDF handbook
   (default: "ON")
 * `BUILD_HANDBOOK_PDF_LANGUAGES`
       - list of language codes to use for PDF handbook, or empty for all
         languages (default: "")
 * `BUILD_HOWTO_HTML` - whether to generate and install the HTML howto (default:
   "ON")
 * `BUILD_HOWTO_HTML_LANGUAGES`
       - list of language codes to use for HTML howto, or empty for all
         languages (default: "")
 * `BUILD_HOWTO_PDF` - whether to generate and install the PDF howto (default:
   "ON")
 * `BUILD_HOWTO_PDF_LANGUAGES`
       - list of language codes to use for PDF howto, or empty for all languages
         (default: "")

For example, passing `-DGENERATE-HOWTO_HTML_LANGUAGES=en;et` to the cmake
command to restricts generation and installation of howto HTML versions to the
English and Estonian translations only, and `-DBUILD_HOWTO_PDF=OFF` disables
generation of the howto in PDF format.


## FEEDBACK

Please report any bugs you find to
[our issue tracker](https://github.com/bibletime/bibletime/issues)
or see [the BibleTime homepage](https://bibletime.info/) for additional information.

## DOCUMENTATION

You can find information about BibleTime, the installation
and its usage in the following places:
 - https://bibletime.info/
 - The BibleTime handbook available in the Help menu of the BibleTime application.
 - There's a Bible study tutorial in the help menu.

May God, our Lord, use BibleTime for his glory.

The BibleTime developers
