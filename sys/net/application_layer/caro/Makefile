SRC := caro_server.c

SUBMODULES := 1

# Since caro extends gcoap, it shall be provided with gcoap internals
INCLUDES += -I$(RIOTBASE)/sys/net/application_layer/nanocoap
INCLUDES += -I$(RIOTBASE)/sys/net/application_layer/gcoap

include $(RIOTBASE)/Makefile.base
