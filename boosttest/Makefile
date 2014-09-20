all: timed_wait_test

clean:
	rm -f timed_wait_test

CPPFLAGS += -DBOOST_TEST_DYN_LINK
LDLIBS += -lpthread
LDLIBS += -lboost_date_time
LDLIBS += -lboost_thread
LDLIBS += -lboost_unit_test_framework
