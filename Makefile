IMID=OVIMTobacco
SOURCES=OVIMTobacco PredictorSingleton DictionarySingleton\
    Token Vocabulary Candidate BiGram
DATA=imtables.db tsi.db
CINS=bpmf_punctuation.cin
LDFLAGS+=-lsqlite3

include Mk/ov.os.mk
include Mk/ov.im.mk
include Mk/ov.vars.mk
include Mk/ov.general.mk

INC+=
VPATH+= .
CFLAGS=-g $(INC)

all: OVIMTobacco.dylib $(DATA)

imtables.db: imtables-schema.txt ${CINS} cin-sqlconvert.pl
	rm -f imtables.db
	sqlite3 imtables.db < imtables-schema.txt
	perl cin-sqlconvert.pl ${CINS} | sqlite3 imtables.db
	
tsi.db: tobacco-schema.txt tobacco-sqlconvert.pl
	rm -f tsi.db
	sqlite3 tsi.db < tobacco-schema.txt
	cat tsi.src | perl -w -CO tobacco-sqlconvert.pl | sqlite3 tsi.db

clean_db:
	rm -f tsi.db
