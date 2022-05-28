#include <iostream>
#include <vector>

#define DAYS_UNTIL 14

class Talkative {
 public:
  virtual void talk() const = 0;
  virtual void talkQueen() const = 0;
};

class TalkativeColony {
 public:
  virtual void talk() const = 0;
};

class TalkativeHeap {
 public:
  virtual void talk() const = 0;
};

class Ant : public Talkative {
 protected:

  int _health;

  int _defence;

  int _damage;

  const char *_typeName;

  const char *_rankName;

  const char *_queenTalk;

  bool _dead = false;

  // 1 - this ant is queen, 0 - this ant isn't queen
  bool _queen = false;

 public:
  void talk() const override {
	std::cout << "Тип: " << this->_typeName << std::endl;
	std::cout << "Ранг: " << this->_rankName << std::endl;
	std::cout << "Параметры: " << "здоровье=" << this->_health << ", защита=" << this->_defence << ", урон="
			  << this->_damage << std::endl;
  }

  void talkQueen() const override {
	std::cout << _queenTalk << std::endl;
  }

  Ant(int health,
	  int defence,
	  int damage,
	  const char *type_name,
	  const char *rank_name,
	  const char *queen_talk,
	  bool queen)
	  : _health(health),
		_defence(defence),
		_damage(damage),
		_typeName(type_name),
		_rankName(rank_name),
		_queenTalk(queen_talk),
		_queen(queen) {}

  Ant(int health, int defence, int damage, const char *type_name, const char *rank_name, const char *queen_talk)
	  : _health(health),
		_defence(defence),
		_damage(damage),
		_typeName(type_name),
		_rankName(rank_name),
		_queenTalk(queen_talk) {}

  int getHealth() const {
	return _health;
  }
  int getDefence() const {
	return _defence;
  }
  int getDamage() const {
	return _damage;
  }
  const char *getTypeName() const {
	return _typeName;
  }
  const char *getRankName() const {
	return _rankName;
  }
  const char *getQueenTalk() const {
	return _queenTalk;
  }
  bool isDead() const {
	return _dead;
  }
  bool isQueen() const {
	return _queen;
  }

};

class Colony : public TalkativeColony {
 protected:
  const char *_name;

  // ants info
  Ant *_queen;
  std::vector<Ant> _workers;
  std::vector<Ant> _warriors;
  std::vector<Ant> _specials;

  // resources info
  int _kamyshek = 0;
  int _vetochka = 0;
  int _listik = 0;
  int _rosinka = 0;

 public:
  void talk() const override {
	std::cout << "Колония: \'" << _name << "\':" << std::endl;
	std::cout << "---" << this->_queen->getQueenTalk() << std::endl;
	std::cout << "---Ресурсы: к=" << this->_kamyshek << ", в=" << this->_vetochka << ", л=" << this->_listik << ", р="
			  << this->_rosinka << std::endl;
	std::cout << "---Популяция " << this->_workers.size() + this->_warriors.size() + this->_specials.size() << ": р="
			  << this->_workers.size() << ", в=" << this->_warriors.size() << ", о=" << this->_specials.size()
			  << std::endl;
  }

  void startWalk() {
	std::cout << "Колония: " << _name << " начала свой поход" << std::endl;
  }

  Colony(const char *name,
		 Ant *queen,
		 const std::vector<Ant> &workers,
		 const std::vector<Ant> &warriors,
		 const std::vector<Ant> &specials)
	  : _name(name), _queen(queen), _workers(workers), _warriors(warriors), _specials(specials) {}
};

class Heap: public TalkativeHeap {
 protected:
  short _number;

  // resources info
  int _kamyshek;
  int _vetochka;
  int _listik;
  int _rosinka;


 public:
  short getNumber() const {
	return _number;
  }
  int getKamyshek() const {
	return _kamyshek;
  }
  int getVetochka() const {
	return _vetochka;
  }
  int getListik() const {
	return _listik;
  }
  int getRosinka() const {
	return _rosinka;
  }

  bool isDead() const {
	return _kamyshek == 0 && _vetochka == 0 && _listik == 0 && _rosinka == 0;
  }

  void talk() const override {
	std::cout << "Куча " << this->_number << ": ";

	if (isDead()) {
	  std::cout << "истощена" << std::endl;
	  return;
	}

	if (_kamyshek > 0)
	  std::cout << "камушек=" << _kamyshek << ", ";

	if (_vetochka > 0)
	  std::cout << "веточка=" << _vetochka << ", ";

	if (_listik > 0)
	  std::cout << "листик=" << _listik << ", ";

	if (_rosinka > 0)
	  std::cout << "росинка=" << _rosinka;

	std::cout << std::endl;
  }

  Heap(short number, int kamyshek, int vetochka, int listik, int rosinka)
	  : _number(number), _kamyshek(kamyshek), _vetochka(vetochka), _listik(listik), _rosinka(rosinka) {}
};

class HeapStorage {
 protected:
  std::vector<Heap> _heaps = *new std::vector<Heap>;

 public:
  void talk() const {
	for (const auto & _heap : _heaps) {
	  _heap.talk();
	}
  }

  int getAliveCount() const {
	int count = 0;

	for (const auto & _heap : _heaps) {
	  if (!_heap.isDead()) count++;
	}

	return count;
  }

  void addHeap(Heap& heap) {
	_heaps.push_back(heap);
  }
};

class AbstractGameProcessor {
 public:
  virtual void nextDay() = 0;

  virtual bool isNextDay() const = 0;

  virtual void talk() const = 0;
};


class GameProcessor: public AbstractGameProcessor{
 protected:
  int _days;

  HeapStorage* _heap_storage;

  Colony* _colony1;
  Colony* _colony2;

 public:
  GameProcessor(int days, HeapStorage *heap_storage, Colony *colony_1, Colony *colony_2)
	  : _days(days), _heap_storage(heap_storage), _colony1(colony_1), _colony2(colony_2) {}

  int getDays() const {
	return _days;
  }
  HeapStorage *getHeapStorage() const {
	return _heap_storage;
  }
  Colony *getColony1() const {
	return _colony1;
  }
  Colony *getColony2() const {
	return _colony2;
  }

  void nextDay() override {
	_days--;
	_colony1->startWalk();
	_colony2->startWalk();
  }
  bool isNextDay() const override {
	return _days > 0;
  }
  void talk() const override {
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "День " << DAYS_UNTIL - _days + 1 << " (до засухи " << _days << " дней):" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	_colony1->talk();
	std::cout << "-------------------------------------------" << std::endl;
	_colony2->talk();
	std::cout << "-------------------------------------------" << std::endl;
	_heap_storage->talk();
	std::cout << "-------------------------------------------" << std::endl;
  }
};

Colony* generateOrangeColony() {
  auto* workers = new std::vector<Ant>;
  auto* warriors = new std::vector<Ant>;
  auto* specials = new std::vector<Ant>;

  Ant* queen = new Ant(
	  24,
	  5,
	  16,
	  "",
	  "",
	  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
	  true
	  );

  return new Colony(
	  "рыжие",
	  queen,
	  *workers,
	  *warriors,
	  *specials
	  );
}

HeapStorage* generateHeapStorage() {
  auto* heap1 = new Heap(
	  1,
	  0,
	  13,
	  49,
	  0
	  );

  auto* heap2 = new Heap(
	  2,
	  43,
	  40,
	  18,
	26
	  );

  auto* heap3 = new Heap(
	  3,
	  21,
	  45,
	  36,
	  19
	  );

  auto* heap4 = new Heap(
	  4,
	  13,
	  23,
	  30,
	  10
	  );

  auto* heap5 = new Heap(
	  5,
	  38,
	  25,
	  0,
	  43
	  );

  auto* hs = new HeapStorage();
  hs->addHeap(*heap1);
  hs->addHeap(*heap2);
  hs->addHeap(*heap3);
  hs->addHeap(*heap4);
  hs->addHeap(*heap5);

  return hs;
}


int main() {
  std::cout << "Человечество уничтожено. Последний оплод цивилизации - муравьи, всего 2 колонии." << std::endl;
  std::cout << "До засухи осталось всего " << DAYS_UNTIL << " дней" << std::endl;
  std::cout << "Выживет только сильнейшая колония, приятного просмотра голодных муравьиных игр" << std::endl;

  GameProcessor* gp = new GameProcessor(
	  	DAYS_UNTIL,
		generateHeapStorage(),
		generateOrangeColony(),
		generateOrangeColony()
	  );

  gp->talk();
  gp->nextDay();
  gp->talk();


  return EXIT_SUCCESS;
}
