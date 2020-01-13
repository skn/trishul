TRISHUL_JAR=../kaffe-1.1.7/trishul/trishul.jar

TAINT_TEST_JAVA_FILES=$(shell find trishul/test/taint -name '*.java' -exec grep -l main '{}' ';')
TAINT_TEST_JASMIN_FILES=$(shell find trishul/test/taint -name '*.j' -exec grep -l main '{}' ';')
TAINT_TEST_CLASS_FILES=$(patsubst %.java,%.class,$(TAINT_TEST_JAVA_FILES)) $(patsubst %.j,%.class,$(TAINT_TEST_JASMIN_FILES))
TAINT_TEST_CLASSES=$(patsubst %.class,%,$(TAINT_TEST_CLASS_FILES))

DECOMPILE_TEST_JAVA_FILES=$(shell find trishul/test/decompile -name '*.java' -exec grep -l main '{}' ';')
DECOMPILE_TEST_CLASS_FILES=$(patsubst %.java,%.class,$(DECOMPILE_TEST_JAVA_FILES))

TRISHUL_TEST_JAVAS=$(wildcard trishul/test/*.java)
TRISHUL_TEST_CLASSES=$(patsubst %.java,%.class,$(TRISHUL_TEST_JAVAS))

POLYMER_TEST_ENGINES=$(shell find trishul/test/trishul_p -name '*.eng')
POLYMER_TEST_ACTIONS=$(shell find trishul/test/trishul_p -name '*.action')
POLYMER_TEST_OUTPUTS=$(shell find trishul/test/trishul_p -name '*.out')

POLYMER_ENGINE_JAVAS=$(patsubst %.action,%.java,$(POLYMER_TEST_ACTIONS)) $(patsubst %.eng,%.java,$(POLYMER_TEST_ENGINES))
POLYMER_TEST_JAVAS=$(shell find trishul/test/trishul_p/ -name '*.java'  -exec grep -l main '{}' ';') \
					$(POLYMER_ENGINE_JAVAS)

POLYMER_TEST_CLASSES=$(patsubst %.java,%.class,$(POLYMER_TEST_JAVAS))
POLYMER_TEST_TMP=test-trishul_p.tmp

OTHER_JAVAS=$(wildcard trishul/test/gtk/*.java)
OTHER_CLASSES=$(patsubst %.java,%.class,$(OTHER_JAVAS))

all: local
	cd webserver && ant

local: $(TRISHUL_TEST_CLASSES) $(patsubst %.action,%.class,$(POLYMER_TEST_ACTIONS)) $(POLYMER_TEST_JAVAS) $(POLYMER_TEST_CLASSES) \
	 $(OTHER_CLASSES) taint_test_classes decompile_test_classes

clean: core_clean
	find trishul -name '*.class' -exec rm '{}' ';'
	rm -f *.class
	find . -name '*.jdasm' -exec rm '{}' ';'
	rm -f $(POLYMER_TEST_TMP) $(POLYMER_ENGINE_JAVAS)

core_clean:
	rm -f core*

test: trishulp_test taint_test

test_release: taint_test_release

taint_test_classes: $(TAINT_TEST_CLASS_FILES)

taint_test: $(TAINT_TEST_CLASS_FILES)
	@echo Executing taint tests
	@for f in $(TAINT_TEST_CLASSES); do echo '  '$$f; ./kaffe-tst -Xtaintdebuglevel 0 $$f; done
	@echo Executed taint tests

taint_test_release: $(TAINT_TEST_CLASS_FILES)
	@echo Executing tests
	@for f in $(TAINT_TEST_CLASSES); do echo '  '$$f; ./kaffe-tst $$f; done
	@echo Executed tests

trishulp_test: $(POLYMER_TEST_CLASSES)
	@echo Executing trishul-p tests
	@for f in $(POLYMER_TEST_OUTPUTS); do ./trishulp-test $$f $(POLYMER_TEST_TMP); done
	@echo Executed trishul-p tests


decompile_test_classes: $(DECOMPILE_TEST_CLASS_FILES)

%.java: %.eng $(TRISHUL_JAR)
	java -classpath .:$(TRISHUL_JAR) trishul_p.compiler.Compile $<

%.java: %.action $(TRISHUL_JAR)
	java -classpath .:$(TRISHUL_JAR) trishul_p.compiler.Compile $<

%.class: %.java
	./javac-asm $<

%.class: %.j
	java -jar jasmin.jar $<
