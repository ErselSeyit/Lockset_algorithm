//Lock.h
#ifndef LOCK_H
#define LOCK_H

class Lock {
private:
    int id;

public:
    Lock(int id);
    int getId() const;
};

#endif // LOCK_H