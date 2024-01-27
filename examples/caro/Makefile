RIOTBASE ?= $(CURDIR)/../..
TARGET?=native

all: pkg server

pkg:
	@IN_GIT="false" ; \
	if [ "$${IN_GIT}" = "true" ] ; then \
		rm -rf pkg_libcoap/patches ; \
		mkdir -p pkg_libcoap/patches ; \
		RIOT_HASH=`grep PKG_VERSION= pkg_libcoap/Makefile | cut -d= -f2` ; \
		git pull --unshallow > /dev/null 2>&1 ; \
		if [ ! -z "$${RIOT_HASH}" ] ; then \
			(cd pkg_libcoap/patches ; git format-patch -n $${RIOT_HASH}) ; \
		fi \
	fi
	rm -rf $(RIOTBASE)/pkg/libcoap && mkdir $(RIOTBASE)/pkg/libcoap
	cd pkg_libcoap && cp -r * $(RIOTBASE)/pkg/libcoap
	@HAVE_KCONFIG=`grep libcoap/Kconfig $(RIOTBASE)/pkg/Kconfig | wc -l` ; \
	if [ "$${HAVE_KCONFIG}" = 0 ] ; then \
		sed -i '/rsource "libcose\/Kconfig"/irsource "libcoap\/Kconfig"' $(RIOTBASE)/pkg/Kconfig ; \
	fi

server: pkg
	$(MAKE) -C server RIOT_CI_BUILD=1
