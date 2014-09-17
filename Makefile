
PROGRAM = $(OUTDIR)/whirl

WU = $(OUTDIR)/wu

WHIRL_VERSION      = 0.1
WHIRL_RCPATTERN    = \".whirl.%s.rc\"
WHIRL_OPTIONS_FILE = \"options\"

OUTDIR = $(shell uname -s | sed "s/SunOS/Solaris/g")_$(shell uname -m)

CC       = gcc
COPT     = 
CSOURCES = pdopen.c  whcurl.c  whdebug.c  wherror.c  whmain.c  whparse.c  whstr.c
COBJECTS = $(CSOURCES:%.c=$(OUTDIR)/%.o)
CFLAGS   = $(CINCS) $(CDEFS)
CINCS    = $(shell curl-config --cflags)
CDEFS    = \
	-DWHIRL_RCPATTERN="$(WHIRL_RCPATTERN)" \
	-DWHRIL_VERSION="$(WHIRL_VERSION)" \
	-DWHIRL_OPTIONS_FILE="$(WHIRL_OPTIONS_FILE)"

LDFLAGS  = $(LDDIRS) $(LDLIBS)
LDDIRS   = -L/usr/kerberos/lib
LDLIBS   = \
	-Wl,-Bstatic \
		-lcurl \
		-lssl \
		-lcrypto \
		-lk5crypto \
		-lcom_err \
		-lidn \
		-lkrb5 \
	-Wl,-Bdynamic \
		-lgssapi_krb5 \
		-ldl \
		-lz
LDCURL   = $(shell curl-config --libs)
xLDCURL   = -lcurl -ldl -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -lidn -lz



all: $(PROGRAM)

$(PROGRAM): $(COBJECTS)
	@echo LINK $@
	@if [ ! -d $(OUTDIR) ] ; then mkdir -p $(OUTDIR) ; fi
	@$(CC) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/%.o: %.c
	@echo CC $<
	@if [ ! -d $(OUTDIR) ] ; then mkdir -p $(OUTDIR) ; fi
	@$(CC) $(COPT) -c $< -o $@ $(CFLAGS)

clean: objclean execlean outclean
objclean:
	$(RM) $(COBJECTS)
execlean:
	$(RM) $(PROGRAM) $(WU)
outclean:
	if [ -d $(OUTDIR) ] ; then rmdir $(OUTDIR) ; fi

$(WU): $(PROGRAM)
	ln -sf $(shell basename $(PROGRAM)) $(WU)

run: $(WU)
	$<
