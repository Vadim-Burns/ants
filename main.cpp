#include <iostream>
#include <vector>
#include <exception>
#include <random>

#define DAYS_UNTIL 14
#define EVENT_DAYS 3

#define WORKER "РАБОЧИЙ"
#define WARRIOR "ВОИН"
#define SPECIAL "ОСОБЫЙ"

#define WORKER_TYPE -1000
#define WORKER_SPECIAL_TYPE -1001
#define WARRIOR_TYPE -1100
#define WARRIOR_SPECIAL_TYPE -1101
#define SPECIAL_TYPE -1200

#define WORKER_ABILITY_STAR -1000
#define WORKER_ABILITY_PROD -1001
#define WORKER_ABILITY_LEG -1002

#define WORKER_ABILITY_PROD1 -1100
#define WORKER_ABILITY_PROD2 -1101
#define WORKER_ABILITY_STARZ -1102

#define ULTIMATE_DAMAGE 1000

#define DEFAULT_RANK "обычный"

int randomN(int min, int max) {
  std::random_device rd;     // only used once to initialise (seed) engine
  std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni(min,max); // guaranteed unbiased

  return uni(rng);
}

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

  int _type;

  bool _dead = false;

  int _worker_ab = 0;

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

  Ant(const int type, const char *queen_talk) {
	switch (type) {
	  case WARRIOR_TYPE: _health = 1;
		_defence = 0;
		_damage = 1;
		_typeName = WARRIOR;
		break;
	  case WORKER_TYPE: _health = 1;
		_defence = 0;
		_damage = 0;
		break;
	  default: throw std::invalid_argument("auto generate only default worker or warrior");
	}

	_rankName = DEFAULT_RANK;
	_queenTalk = queen_talk;
	_type = type;
  }

  Ant(int health, int defence, int damage, const char *rank_name, const char *queen_talk, const int type)
	  : _health(health),
		_defence(defence),
		_damage(damage),
		_rankName(rank_name),
		_queenTalk(queen_talk),
		_type(type) {
	switch (type) {
	  case WORKER_TYPE:
	  case WORKER_SPECIAL_TYPE: _typeName = WORKER;
		break;
	  case WARRIOR_TYPE:
	  case WARRIOR_SPECIAL_TYPE: _typeName = WARRIOR;
		break;
	  default: _typeName = SPECIAL;
	}
  }

  Ant(int health,
	  int defence,
	  int damage,
	  const char *rank_name,
	  const char *queen_talk,
	  const int type,
	  const int w_ab)
	  : _health(health),
		_defence(defence),
		_damage(damage),
		_rankName(rank_name),
		_queenTalk(queen_talk),
		_type(type),
		_worker_ab(w_ab) {
	switch (type) {
	  case WORKER_TYPE:
	  case WORKER_SPECIAL_TYPE: _typeName = WORKER;
		break;
	  case WARRIOR_TYPE:
	  case WARRIOR_SPECIAL_TYPE: _typeName = WARRIOR;
		break;
	  default: _typeName = SPECIAL;
	}
  }

  int getDamage() const {
	return _damage;
  }

  int getType() const {
	return _type;
  }

  // false - not dead, true - dead
  bool getBeat(int damage) {
	if (damage == ULTIMATE_DAMAGE) {
	  _dead = true;
	  return true;
	}
	int hp = _health - damage + _defence;
	if (hp > 0) {
	  _health = hp;
	  _dead = false;
	  return false;
	}

	return true;
  }

  int getWorkerAb() const {
	return _worker_ab;
  }

};

class Queen {
 protected:
  const char *_name;
  const char *_queenTalk;

  // количество дней высидки
  const int _period_max;
  const int _period_min;

  // количество личинок
  const int _count_ch_min;
  const int _count_ch_max;

  // сколько дней прошло с последних личнок
  int _cur_day = 0;
  bool _ready = false;

  bool isChildrenReady() {
	if (!_ready)
	  _ready = _cur_day >= _period_min && (randomN(_cur_day, _period_max) == _cur_day);
	return _ready;
  }

 public:
  Queen(const char *name,
		const char *queen_talk,
		const int period_max,
		const int period_min,
		const int count_ch_min,
		const int count_ch_max)
	  : _name(name),
		_queenTalk(queen_talk),
		_period_max(period_max),
		_period_min(period_min),
		_count_ch_min(count_ch_min),
		_count_ch_max(count_ch_max) {}

  const char *getQueenTalk() const {
	return _queenTalk;
  }

  void nextDay() {
	_cur_day++;
  }

  int getDaysBefore() {
	return _period_max - _cur_day;
  }

  std::vector<Ant> *getChildren() {
	if (!isChildrenReady()) return nullptr;

	auto *children = new std::vector<Ant>;

	int count_ch = randomN(_count_ch_min, _count_ch_max);

	for (int i = 0; i < count_ch; i++) {
	  children->push_back(
		  *new Ant(
			  (randomN(0, 1) ? WARRIOR_TYPE : WORKER_TYPE),
			  _queenTalk
		  )
	  );
	}

	_cur_day = 0;
	_ready = false;

	return children;
  }

};

class Colony : public TalkativeColony {
 protected:
  const char *_name;

  // ants info
  Queen *_queen;
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

  void childrenNextDay() {
	_queen->nextDay();
  }

  void getChildren() {
	auto *children = _queen->getChildren();

	if (children == nullptr) {
	  std::cout << "---Потомство: еще не готовы(осталось не больше " << _queen->getDaysBefore() << " дней)"
				<< std::endl;
	  return;
	}

	int count_warriors = 0;
	int count_workers = 0;

	for (int i = 0; i < children->size(); i++) {
	  Ant ant = (*children)[i];
	  if (ant.getType() == WARRIOR_TYPE) {
		count_warriors++;
		_warriors.push_back(ant);
	  } else {
		count_workers++;
		_workers.push_back(ant);
	  }
	}

	delete children;

	std::cout << "---Потомство: р=" << count_workers << ", в=" << count_warriors << std::endl;
  }

  Colony(const char *name,
		 Queen *queen,
		 const std::vector<Ant> &workers,
		 const std::vector<Ant> &warriors,
		 const std::vector<Ant> &specials)
	  : _name(name), _queen(queen), _workers(workers), _warriors(warriors), _specials(specials) {}

  const char *getName() const {
	return _name;
  }

  int getWorkersSize() {
	return _workers.size();
  }

  int getWarriorsSize() {
	return _warriors.size();
  }

  int getSpecialsSize() {
	return _specials.size();
  }

  Ant &getWorker(int index) {
	return _workers[index];
  }

  Ant &getWarrior(int index) {
	return _warriors[index];
  }

  Ant &getSpecial(int index) {
	return _specials[index];
  }

  void killWorker(int index) {
	_workers.erase(_workers.begin() + index);
  }

  void killWarrior(int index) {
	_warriors.erase(_warriors.begin() + index);
  }

  void killSpecial(int index) {
	_specials.erase(_specials.begin() + index);
  }

  void killWarriors(const std::vector<int> &indexes) {
	int cnt = 0;

	for (auto i : indexes) {
	  killWarrior(i - cnt);
	  cnt++;
	}
  }

  void killWorkers(const std::vector<int> &indexes) {
	int cnt = 0;

	for (auto i : indexes) {
	  killWorker(i - cnt);
	  cnt++;
	}
  }

  void killSpecials(const std::vector<int> &indexes) {
	int cnt = 0;

	for (auto i : indexes) {
	  killSpecial(i - cnt);
	  cnt++;
	}
  }

  int resSum() const {
	return _kamyshek + _listik + _rosinka + _vetochka;
  }

  bool hasDisableWorkerAb() {
	for (auto &a : _workers) {
	  if (a.getWorkerAb() == WORKER_ABILITY_LEG) return true;
	}
	return false;
  }

  void addResources(int kamyshek, int listik, int vetochka, int rosinka) {
	_kamyshek += kamyshek;
	_listik += listik;
	_vetochka += vetochka;
	_rosinka += rosinka;
  }
};

class Heap : public TalkativeHeap {
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

  void takeKamyshek() {
	_kamyshek--;
  }

  void takeLisktik() {
	_listik--;
  }

  void takeVetochka() {
	_vetochka--;
  }

  void takeRosinka() {
	_rosinka--;
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
	for (const auto &_heap : _heaps) {
	  _heap.talk();
	}
  }

  int getAliveCount() const {
	int count = 0;

	for (const auto &_heap : _heaps) {
	  if (!_heap.isDead()) count++;
	}

	return count;
  }

  Heap *getRandomAliveHeap() {
	if (getAliveCount() == 0) return nullptr;

	int heap_n = randomN(0, _heaps.size() - 1);

	Heap *cur_heap = &_heaps[heap_n];
	while (cur_heap->isDead()) {
	  heap_n = (heap_n + 1) % _heaps.size();
	  cur_heap = &_heaps[heap_n];
	}

	return cur_heap;
  }

  void addHeap(Heap &heap) {
	_heaps.push_back(heap);
  }
};

class AbstractGameProcessor {
 public:
  virtual void nextDay() = 0;

  virtual void talk() const = 0;
};

class GameProcessor : public AbstractGameProcessor {
 protected:
  int _days;

  HeapStorage *_heap_storage;

  Colony *_colony1;
  Colony *_colony2;

  bool _specials = true;
  bool _event_happened = false;
  short _event_day = 0;

  void startBattle() {
	std::cout << "Обе колонии выбрали одну кучу! Начинается битва" << std::endl;

	std::vector<int> wor1;
	std::vector<int> wor2;

	std::vector<int> war1;
	std::vector<int> war2;

	std::vector<int> scp1;
	std::vector<int> scp2;

	if (_colony1->getWarriorsSize() > _colony2->getWarriorsSize()) {
	  int i;
	  for (i = 0; i < _colony2->getWarriorsSize(); i++) {
		if (_colony1->getWarrior(i).getBeat(_colony2->getWarrior(i).getDamage())) war1.push_back(i);

		if (_colony2->getWarrior(i).getBeat(_colony1->getWarrior(i).getDamage())) war2.push_back(i);
	  }

	  if (i < _colony1->getWarriorsSize()) {
		for (int j = 0; i < _colony1->getWarriorsSize() && j < _colony2->getWorkersSize(); i++, j++) {
		  wor2.push_back(j);
		}
	  }
	} else {
	  int i;
	  for (i = 0; i < _colony1->getWarriorsSize(); i++) {
		if (_colony1->getWarrior(i).getBeat(_colony2->getWarrior(i).getDamage())) war1.push_back(i);

		if (_colony2->getWarrior(i).getBeat(_colony1->getWarrior(i).getDamage())) war2.push_back(i);
	  }

	  if (i < _colony2->getWarriorsSize()) {
		for (int j = 0; i < _colony2->getWarriorsSize() && j < _colony1->getWorkersSize(); i++, j++) {
		  wor1.push_back(j);
		}
	  }
	}

	// особые особи, обладая повышенным интеллектом атакуют в первую очередь особых и рабочих, так как войны не имеют
	// для них значения
	if ((_colony1->getSpecialsSize() > 0 || _colony2->getSpecialsSize() > 0) && _specials) {
	  if (_colony2->getSpecialsSize() > 0) {
		// атака второй колонии
		if (_colony1->getSpecialsSize() > 0) {
		  if (_colony1->getSpecial(0).getBeat(_colony2->getSpecial(0).getDamage())) scp1.push_back(0);
		} else if (_colony1->getWorkersSize() - wor1.size() > 0) {
		  wor1.push_back(_colony1->getWorkersSize() - 1);
		} else if (_colony1->getWarriorsSize() - war1.size() > 0) {
		  if (_colony1->getWarrior(_colony1->getWarriorsSize() - 1).getBeat(_colony2->getSpecial(0).getDamage()))
			war1.push_back(_colony1->getWarriorsSize() - 1);
		}
	  }

	  // атака первой колонии
	  if (_colony1->getSpecialsSize() > 0) {
		int attcks = 0;
		if (_colony2->getSpecialsSize() > 0) {
		  if (_colony2->getSpecial(0).getBeat(_colony1->getSpecial(0).getDamage())) scp2.push_back(0);
		  attcks++;
		}
		if (attcks < 2 && _colony2->getWorkersSize() - wor2.size() > 0) {
		  attcks++;
		  wor2.push_back(_colony2->getWorkersSize() - 1);
		}
		if (attcks < 2 && _colony2->getWorkersSize() - wor2.size() > 0) {
		  attcks++;
		  wor2.push_back(_colony2->getWorkersSize() - 2);
		}

		if (attcks < 2 && _colony2->getWarriorsSize() - war2.size() > 0) {
		  attcks++;
		  if (_colony2->getWarrior(_colony2->getWarriorsSize() - 1).getBeat(_colony1->getSpecial(0).getDamage()))
			war2.push_back(_colony2->getWarriorsSize() - 1);
		}
		if (attcks < 2 && _colony2->getWarriorsSize() - war2.size() > 0) {
		  attcks++;
		  if (_colony2->getWarrior(_colony2->getWarriorsSize() - 2).getBeat(_colony1->getSpecial(0).getDamage()))
			war2.push_back(_colony2->getWarriorsSize() - 2);
		}
	  }
	}

	std::cout << "Битва окончена." << std::endl;
	std::cout << "Колония \'" << _colony1->getName() << "\' потеряла:р=" << wor1.size() << ", в=" << war1.size();
	if (_specials) std::cout << ", о=" << scp1.size();
	std::cout << std::endl;
	std::cout << "Колония \'" << _colony2->getName() << "\' потеряла:р=" << wor2.size() << ", в=" << war2.size();
	if (_specials) std::cout  << ", о=" << scp2.size();
	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;

	_colony1->killWarriors(war1);
	_colony2->killWarriors(war2);

	_colony1->killWorkers(wor1);
	_colony2->killWorkers(wor2);

	_colony1->killSpecials(scp1);
	_colony2->killSpecials(scp2);
  }

  void showWalkInfo(Heap *heap1, Heap *heap2) {
	std::cout << std::endl;

	if (heap1 == nullptr) {
	  std::cout << "Походы более невозможны по причине истощения куч";
	  return;
	}

	std::cout << "Колония \'" << _colony1->getName() << "\' начала поход на кучу " << heap1->getNumber();
	std::cout << ": р=" << _colony1->getWorkersSize() << ", в=" << _colony1->getWarriorsSize();
	if (_specials) std::cout << ", o=" << _colony1->getSpecialsSize();
	std::cout << std::endl;

	std::cout << "Колония \'" << _colony2->getName() << "\' начала поход на кучу " << heap2->getNumber();
	std::cout << ": р=" << _colony2->getWorkersSize() << ", в=" << _colony2->getWarriorsSize();
	if (_specials) std::cout << ", о=" << _colony2->getSpecialsSize();
	std::cout << std::endl;
  }

  static std::vector<int>& proccesResourcesTake(Heap* heap, const Ant& ant, bool abs) {
	// 0 - vetochka, 1 - listik, 2 - kamyshek, 3 - rosinka
	std::vector<int>& a = *new std::vector<int>;
	a.push_back(0);
	a.push_back(0);
	a.push_back(0);
	a.push_back(0);

	int cnt = 0;
	switch (abs ? ant.getWorkerAb() : 0) {
	  case WORKER_ABILITY_STAR:
		if (heap->getVetochka() > 0) {
		  a[0]++;
		  heap->takeVetochka();
		}
		break;
	  case WORKER_ABILITY_PROD:
		if (heap->getVetochka() > 0) {
		  a[0]++;
		  cnt++;
		  heap->takeVetochka();
		}

		if (heap->getKamyshek() > 0) {
		  a[2]++;
		  cnt++;
		  heap->takeKamyshek();
		}

		if (heap->getVetochka() > 0 && cnt < 2) {
		  a[0]++;
		  cnt++;
		  heap->takeVetochka();
		}

		if (heap->getKamyshek() > 0 && cnt < 2) {
		  a[2]++;
		  cnt++;
		  heap->takeKamyshek();
		}

		break;
	  case WORKER_ABILITY_LEG:
		if (heap->getKamyshek() > 0) {
		  a[2]++;
		  heap->takeKamyshek();
		}

		if (heap->getListik() > 0) {
		  a[1]++;
		  heap->takeLisktik();
		}

		if (heap->getVetochka() > 0) {
		  a[0]++;
		  heap->takeVetochka();
		}

		break;
	  case WORKER_ABILITY_PROD1:
		if (heap->getListik() > 0) {
		  a[1]++;
		  cnt++;
		  heap->takeVetochka();
		}

		if (heap->getKamyshek() > 0) {
		  a[2]++;
		  cnt++;
		  heap->takeKamyshek();
		}

		if (heap->getListik() > 0 && cnt < 2) {
		  a[1]++;
		  cnt++;
		  heap->takeVetochka();
		}

		if (heap->getKamyshek() > 0 && cnt < 2) {
		  a[2]++;
		  cnt++;
		  heap->takeKamyshek();
		}

		break;
	  case WORKER_ABILITY_PROD2:
		if (heap->getRosinka() > 0) {
		  a[3]++;
		  cnt++;
		  heap->takeVetochka();
		}

		if (heap->getKamyshek() > 0) {
		  a[2]++;
		  cnt++;
		  heap->takeKamyshek();
		}

		if (heap->getRosinka() > 0 && cnt < 2) {
		  a[3]++;
		  cnt++;
		  heap->takeVetochka();
		}

		if (heap->getKamyshek() > 0 && cnt < 2) {
		  a[2]++;
		  cnt++;
		  heap->takeKamyshek();
		}

		break;
	  case WORKER_ABILITY_STARZ:
		if (heap->getListik() > 0) {
		  if (randomN(0, 1) == 1) {
			a[1]++;
			heap->takeLisktik();
		  }
		} else if (heap->getVetochka() > 0) {
		  if (randomN(0, 1) == 1) {
			a[0]++;
			heap->takeVetochka();
		  }
		}
	  default:
		if (heap->getKamyshek() > 0) {
		  a[2]++;
		  heap->takeKamyshek();
		} else if (heap->getListik() > 0) {
		  a[1]++;
		  heap->takeLisktik();
		} else if (heap->getVetochka() > 0) {
		  a[0]++;
		  heap->takeVetochka();
		} else if (heap->getRosinka() > 0) {
		  a[3]++;
		  heap->takeRosinka();
		}
	}

	return a;
  }

  void startWalk() {
	Heap *heap1 = _heap_storage->getRandomAliveHeap();
	Heap *heap2 = _heap_storage->getRandomAliveHeap();

	showWalkInfo(heap1, heap2);

	if (heap1 == nullptr) {
	  std::cout << std::endl;
	  return;
	}

	if (heap1 == heap2) {
	  startBattle();
	}

	int kamyshek1 = 0;
	int kamyshek2 = 0;

	int rosinka1 = 0;
	int rosinka2 = 0;

	int vetochka1 = 0;
	int vetochka2 = 0;

	int listik1 = 0;
	int listik2 = 0;

	int i = 0;
	int j = 0;

	bool abs = !(_colony1->hasDisableWorkerAb() || _colony2->hasDisableWorkerAb());

	while (i < _colony1->getWorkersSize() || j < _colony2->getWorkersSize()) {
	  if (heap1->isDead()) {
		break;
	  }

	  if (i < _colony1->getWorkersSize()) {
		Ant ant1 = _colony1->getWorker(i);

		auto a = proccesResourcesTake(heap1, ant1, abs);
		vetochka1 += a[0];
		listik1 += a[1];
		kamyshek1 += a[2];
		rosinka1 += a[3];

		i++;
	  }

	  if (j < _colony2->getWorkersSize()) {
		Ant ant2 = _colony2->getWorker(j);

		auto a = proccesResourcesTake(heap2, ant2, abs);
		vetochka2 += a[0];
		listik2 += a[1];
		kamyshek2 += a[2];
		rosinka2 += a[3];

		j++;
	  }
	}

	if (_colony2->getSpecialsSize() > 0 && _specials) {
	  int cnt = 0;

	  if (heap2->getKamyshek() > 0 && cnt < 2) {
		kamyshek2++;
		heap2->takeKamyshek();
		cnt++;
	  }

	  if (heap2->getRosinka() > 0 && cnt < 2) {
		rosinka2++;
		heap2->takeRosinka();
		cnt++;
	  }

	  if (heap2->getVetochka() > 0 && cnt < 2) {
		vetochka2++;
		heap2->takeVetochka();
		cnt++;
	  }

	  if (heap2->getListik() > 0 && cnt < 2) {
		listik2++;
		heap2->takeLisktik();
		cnt++;
	  }

	  if (heap2->getKamyshek() > 0 && cnt < 2) {
		kamyshek2++;
		heap2->takeKamyshek();
		cnt++;
	  }

	  if (heap2->getRosinka() > 0 && cnt < 2) {
		rosinka2++;
		heap2->takeRosinka();
		cnt++;
	  }

	  if (heap2->getVetochka() > 0 && cnt < 2) {
		vetochka2++;
		heap2->takeVetochka();
		cnt++;
	  }

	  if (heap2->getListik() > 0 && cnt < 2) {
		listik2++;
		heap2->takeLisktik();
		cnt++;
	  }
	}

	_colony1->addResources(kamyshek1, listik1, vetochka1, rosinka1);
	_colony2->addResources(kamyshek2, listik2, vetochka2, rosinka2);

	std::cout << "---Колония \'" << _colony1->getName() << "\' добыла ресурсов: в=" << vetochka1 << ", к=" << kamyshek1;
	std::cout << ", л=" << listik1 << ", р=" << rosinka1 << std::endl;

	std::cout << "---Колония \'" << _colony2->getName() << "\' добыла ресурсов: в=" << vetochka2 << ", к=" << kamyshek2;
	std::cout << ", л=" << listik2 << ", р=" << rosinka2 << std::endl;

	std::cout << std::endl;
  }

  void childrenUpdate() {
	_colony1->childrenNextDay();
	_colony2->childrenNextDay();
  }

 public:
  GameProcessor(int days, HeapStorage *heap_storage, Colony *colony_1, Colony *colony_2)
	  : _days(days), _heap_storage(heap_storage), _colony1(colony_1), _colony2(colony_2) {}

  void updateEvent() {
	if (_event_happened) {
	  if (_event_day == EVENT_DAYS) {
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "Ливень закончился." << std::endl;
		_specials = true;
		_event_day++;
		std::cout << "---------------------------------------------" << std::endl;
	  } else if (_event_day < EVENT_DAYS) {
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "Ливень будет идти еще " << EVENT_DAYS - _event_day << " дней"  << std::endl;
		_event_day++;
		std::cout << "---------------------------------------------" << std::endl;
	  }
	  return;
	}

	if (randomN(0, 1) == 1) {
	  _specials = false;
	  _event_happened = true;
	  _event_day++;

	  std::cout << "---------------------------------------------" << std::endl;
	  std::cout << "Начался ливень, который продлится 3 дня." << std::endl;
	  std::cout << "Все особенные сидят дома!" << std::endl;
	  std::cout << "---------------------------------------------" << std::endl;
	}
  }

  void nextDay() override {
	_days--;

	updateEvent();

	startWalk();

	childrenUpdate();
  }

  void talk() const override {
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "День " << DAYS_UNTIL - _days + 1 << " (до засухи " << _days << " дней):" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	_colony1->talk();
	_colony1->getChildren();
	std::cout << "-------------------------------------------" << std::endl;
	_colony2->talk();
	_colony2->getChildren();
	std::cout << "-------------------------------------------" << std::endl;
	_heap_storage->talk();
	std::cout << "-------------------------------------------" << std::endl;
  }

  const char *getWinnerName() {
	return _colony1->resSum() > _colony2->resSum() ? _colony1->getName() : _colony2->getName();
  }

  void winnerTalk() {
	if (_colony1->resSum() > _colony2->resSum()) {
	  _colony1->talk();
	} else {
	  _colony2->talk();
	}
  }
};

std::vector<Ant> *generateWorkers(int size, const char *queen_talk) {
  auto *workers = new std::vector<Ant>;

  for (int i = 0; i < size; i++) {
	workers->push_back(
		*new Ant(
			WORKER_TYPE,
			queen_talk
		)
	);
  }

  return workers;
}

std::vector<Ant> *generateWarriors(int size, const char *queen_talk) {
  auto *warriors = new std::vector<Ant>;

  for (int i = 0; i < size; i++) {
	warriors->push_back(
		*new Ant(
			WARRIOR_TYPE,
			queen_talk
		)
	);
  }

  return warriors;
}

Colony *generateOrangeColony() {
  auto *workers = generateWorkers(15, "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев");
  auto *warriors = generateWarriors(8, "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев");
  auto *specials = new std::vector<Ant>;

  workers->push_back(
	  *new Ant(
		  2,
		  1,
		  0,
		  "старший",
		  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
		 WORKER_SPECIAL_TYPE,
		 WORKER_ABILITY_STAR
		  )
	  );

  workers->push_back(
	  *new Ant(
		  6,
		  2,
		  0,
		  "продвинутый",
		  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
		  WORKER_SPECIAL_TYPE,
		  WORKER_ABILITY_PROD
	  )
  );

  workers->push_back(
	  *new Ant(
		  10,
		  6,
		  0,
		  "легендарный ветеран",
		  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
		  WORKER_SPECIAL_TYPE,
		  WORKER_ABILITY_LEG
	  )
  );

  warriors->push_back(
	  *new Ant(
		  8,
		  4,
		  5,
		  "элитный легенда",
		  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
		  WARRIOR_TYPE
	  )
  );

  specials->push_back(
	  *new Ant(
		  21,
		  9,
		  8,
		  "ленивый неуязвимый агрессивный крепкий - Шмель",
		  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
		  SPECIAL_TYPE
	  )
  );

  Queen *queen = new Queen(
	  "Беатрикс",
	  "Королева <Беатрикс>, цикл роста личинок 2-4 дней, может создать 1-5 королев",
	  4,
	  2,
	  1,
	  5
  );

  return new Colony(
	  "рыжие",
	  queen,
	  *workers,
	  *warriors,
	  *specials
  );
}

Colony *generateBlackColony() {
  auto *workers = generateWorkers(18, "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев");
  auto *warriors = generateWarriors(5, "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев");
  auto *specials = new std::vector<Ant>;


  warriors->push_back(
	  *new Ant(
		  6,
		  2,
		  0,
		  "продвинутый",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  WORKER_SPECIAL_TYPE,
		  WORKER_ABILITY_PROD1
	  )
  );

  warriors->push_back(
	  *new Ant(
		  6,
		  2,
		  0,
		  "продвинутый",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  WORKER_SPECIAL_TYPE,
		  WORKER_ABILITY_PROD2
	  )
  );

  warriors->push_back(
	  *new Ant(
		  2,
		  1,
		  0,
		  "старший забывчивый",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  WORKER_SPECIAL_TYPE,
		  WORKER_ABILITY_STARZ
	  )
  );

  warriors->push_back(
	  *new Ant(
		  2,
		  1,
		  2,
		  "старший",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  WARRIOR_TYPE
	  )
  );

  warriors->push_back(
	  *new Ant(
		  8,
		  4,
		  5,
		  "элитный",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  WARRIOR_TYPE
	  )
  );

  warriors->push_back(
	  *new Ant(
		  10,
		  6,
		  4,
		  "легендарный мифический",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  WARRIOR_TYPE
	  )
  );

  specials->push_back(
	  *new Ant(
		  29,
		  6,
		  28,
		  "трудолюбивый неуязвимый агрессивный сосредоточенный невезучий - Толстоножка",
		  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
		  SPECIAL_TYPE
	  )
  );

  Queen *queen = new Queen(
	  "Жозефина",
	  "Королева <Жозефина>, цикл роста личинок 3-3 дней, может создать 1-5 королев",
	  3,
	  3,
	  1,
	  5
  );

  return new Colony(
	  "черные",
	  queen,
	  *workers,
	  *warriors,
	  *specials
  );
}

HeapStorage *generateHeapStorage() {
  auto *heap1 = new Heap(
	  1,
	  0,
	  13,
	  49,
	  0
  );

  auto *heap2 = new Heap(
	  2,
	  43,
	  40,
	  18,
	  26
  );

  auto *heap3 = new Heap(
	  3,
	  21,
	  45,
	  36,
	  19
  );

  auto *heap4 = new Heap(
	  4,
	  13,
	  23,
	  30,
	  10
  );

  auto *heap5 = new Heap(
	  5,
	  38,
	  25,
	  0,
	  43
  );

  auto *hs = new HeapStorage();
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

  auto *gp = new GameProcessor(
	  DAYS_UNTIL,
	  generateHeapStorage(),
	  generateOrangeColony(),
	  generateBlackColony()
  );

  for (int i = 0; i < 14; i++) {
	gp->talk();
	gp->nextDay();
  }

  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << "Началась засуха.\nВыжила колония \'" << gp->getWinnerName() << "\'.\nИгра окончена\n";
  gp->winnerTalk();
  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << "----------------------------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
