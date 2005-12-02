
if HAVE_SSL
NETEMBRYO_CERT_FILE = cert.pem
NETEMBRYO_CERT_DIR = $(DESTDIR)@netembryo_state_dir@

EXTRA_DIST = 

all:	cert.pem 

cert.pem: 
	@if test -f $(NETEMBRYO_CERT_DIR)/$(NETEMBRYO_CERT_FILE); then \
		echo "Found $(NETEMBRYO_CERT_DIR)/$(NETEMBRYO_CERT_FILE)."; \
	else \
		echo "*************************************************************************************************************************************";\
		echo "	Please create a cert.pem file and put it in $(NETEMBRYO_CERT_DIR)/ ";\
		echo "*************************************************************************************************************************************";\
	fi

clean:

install-data-local:
	@if test -f $(NETEMBRYO_CERT_DIR)/$(NETEMBRYO_CERT_FILE); then \
		echo "Found $(NETEMBRYO_CERT_DIR)/$(NETEMBRYO_CERT_FILE)."; \
	else \
		if test ! -d $(NETEMBRYO_CERT_DIR); then \
			mkdir -p $(NETEMBRYO_CERT_DIR); \
		fi; \
		echo "*************************************************************************************************************************************";\
		echo "	Please create a cert.pem file and put it in $(NETEMBRYO_CERT_DIR)/ ";\
		echo "*************************************************************************************************************************************";\
	fi
endif
