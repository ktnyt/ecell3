SUBDIRS = users-manual samples

EXTRA_DIST = \
	ecell3.doxy.in \
	index.tmpl

if BUILD_API_REF_html
_htmldir = $(htmldir)/api
_html_DATA = $(wildcard api/*)

api/index.html: ecell3.doxy
	doxygen ecell3.doxy

clean-local:
	$(RM) -r api

distclean-local:
	$(RM) -f api
endif
