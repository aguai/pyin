
PLUGIN_EXT	?= .so

CXX	?= g++
CC	?= gcc

CFLAGS := $(CFLAGS) 
CXXFLAGS := -I. $(CXXFLAGS)

PLUGIN := pyin$(PLUGIN_EXT)

SOURCES := PYinVamp.cpp \
           YinVamp.cpp \
           LocalCandidatePYIN.cpp \
           Yin.cpp \
           YinUtil.cpp \
           MonoNote.cpp \
           MonoNoteParameters.cpp \
           SparseHMM.cpp \
           MonoNoteHMM.cpp \
           MonoPitchHMM.cpp \

PLUGIN_MAIN := libmain.cpp

TESTS := test/test-meanfilter \
         test/test-fft \
         test/test-yin \
         test/test-mononote
	 
OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)

PLUGIN_OBJECTS := $(OBJECTS) $(PLUGIN_MAIN:.cpp=.o)

all: $(PLUGIN) $(TESTS)
	@for t in $(TESTS); do echo; echo "Running $$t"; ./"$$t" || exit 1; done
	@echo; bash ./test/regression/regression.sh

plugin: $(PLUGIN)

$(PLUGIN): $(PLUGIN_OBJECTS)
	$(CXX) -o $@ $^ $(PLUGIN_LDFLAGS)

test/test-meanfilter: test/TestMeanFilter.o $(OBJECTS)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS)

test/test-fft: test/TestFFT.o $(OBJECTS)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS)
	
test/test-yin: test/TestYin.o $(OBJECTS)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS)
	
test/test-mononote: test/TestMonoNote.o $(OBJECTS)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS)

clean:		
		rm -f $(PLUGIN_OBJECTS) test/*.o

distclean:	clean
		rm -f $(PLUGIN) $(TESTS)

depend:
		makedepend -Y -fMakefile.inc *.cpp test/*.cpp *.h test/*.h

# DO NOT DELETE

libmain.o: PYinVamp.h Yin.h MeanFilter.h YinVamp.h LocalCandidatePYIN.h
LocalCandidatePYIN.o: LocalCandidatePYIN.h Yin.h MeanFilter.h
LocalCandidatePYIN.o: MonoPitchHMM.h SparseHMM.h YinUtil.h
MonoNote.o: MonoNote.h MonoNoteHMM.h MonoNoteParameters.h SparseHMM.h
MonoNoteHMM.o: MonoNoteHMM.h MonoNoteParameters.h SparseHMM.h
MonoNoteParameters.o: MonoNoteParameters.h
MonoPitchHMM.o: MonoPitchHMM.h SparseHMM.h
PYinVamp.o: PYinVamp.h Yin.h MeanFilter.h MonoNote.h MonoNoteHMM.h
PYinVamp.o: MonoNoteParameters.h SparseHMM.h MonoPitchHMM.h
SparseHMM.o: SparseHMM.h
Yin.o: Yin.h MeanFilter.h YinUtil.h
YinUtil.o: YinUtil.h MeanFilter.h
YinVamp.o: YinVamp.h Yin.h MeanFilter.h MonoNote.h MonoNoteHMM.h
YinVamp.o: MonoNoteParameters.h SparseHMM.h
YinVampFreqConstrained.o: YinVampFreqConstrained.h Yin.h MeanFilter.h
YinVampFreqConstrained.o: MonoNote.h MonoNoteHMM.h MonoNoteParameters.h
YinVampFreqConstrained.o: SparseHMM.h
test/TestMeanFilter.o: MeanFilter.h
test/TestMonoNote.o: MonoNote.h MonoNoteHMM.h MonoNoteParameters.h
test/TestMonoNote.o: SparseHMM.h
test/TestYin.o: Yin.h MeanFilter.h
test/TestYinUtil.o: YinUtil.h MeanFilter.h
LocalCandidatePYIN.o: Yin.h MeanFilter.h
MonoNote.o: MonoNoteHMM.h MonoNoteParameters.h SparseHMM.h
MonoNoteHMM.o: MonoNoteParameters.h SparseHMM.h
MonoPitchHMM.o: SparseHMM.h
PYinVamp.o: Yin.h MeanFilter.h
Yin.o: MeanFilter.h
YinUtil.o: MeanFilter.h
YinVampFreqConstrained.o: Yin.h MeanFilter.h
YinVamp.o: Yin.h MeanFilter.h
