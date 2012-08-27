cc = g++
FLAGS = `pkg-config --libs opencv --cflags opencv`
#FLAGS += -I /usr/include/mysql -l mysqlclient -L /usr/lib/mysql -lz -lm
FLAGS += -fPIC -g

object = om.o
object += file.o
object += query.o
object += feature.o
object += dbconn.o
object += ominterface.o
object += utils.o
object += harris.o
object += dct.o
#all: so test
all : test
test: $(object) main.o
	$(cc) -o a.out $(object) main.o $(FLAGS)
so:$(object)
	$(cc) -shared -o ../om.so $(object) $(FLAGS)
main.o : main.cc
	$(cc) $(FLAGS) -c $<
om.o : om.cc
	$(cc) $(FLAGS) -c $<
utils.o : utils.cc
	$(cc) $(FLAGS) -c $<
file.o : file.cc
	$(cc) $(FLAGS) -c $<
query.o : query.cc
	$(cc) $(FLAGS) -c $<
feature.o : feature.cc
	$(cc) $(FLAGS) -c $<
dbconn.o : dbconn.cc
	$(cc) $(FLAGS) -c $<
ominterface.o : ominterface.cc
	$(cc) $(FLAGS) -c $<
harris.o : harris.cc
	$(cc) $(FLAGS) -c $<
dct.o : dct.cc
	$(cc) $(FLAGS) -c $<

clean:
	rm $(object) main.o ./a.out
