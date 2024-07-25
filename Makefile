IDIR =./include
CC=g++
CFLAGS=-std=c++23 -ggdb -I$(IDIR) -I/home/cv/Downloads/DataFrame/include -I/usr/local/include/LuaCpp -I/usr/include/lua5.4/ -I/usr/local/include -L/usr/local/lib

ODIR=src/obj
LDIR =./lib

#LIBS=-lta_lib -lpthread
LIBS=-ltbb -lpthread -lcxxtools -ldl -lluacpp -llua5.4

STATIC_LIBS=/usr/local/lib/libDataFrame.a $(LDIR)/libftxui-grid-container.a $(LDIR)/libindicators.a $(LDIR)/libftxui-component.a $(LDIR)/libftxui-dom.a $(LDIR)/libftxui-screen.a

_DEPS = backtest.h date.h tickloader.h TickData.h csv.h SystemRunner.h IndicatorDef.h Triggers.h Exits.h Verbs.h TradeStats.h utils.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = backtest.o tickloader.o TickData.o SystemRunner.o IndicatorDef.o Triggers.o Verbs.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

backtest: $(OBJ)
	$(CC) -o ./src/$@ $^ $(CFLAGS) $(LIBS) $(STATIC_LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

Debug: backtest

