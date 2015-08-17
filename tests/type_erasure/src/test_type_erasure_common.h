#pragma once

// Robot

struct IRobot
{
    virtual ~IRobot() {}
    virtual void Move(int dx, int dy) = 0;
    virtual int GetHealth() const = 0;
    int& mana;

    template <class Object>
    IRobot(Object& obj)
        : mana(obj.mana)
    {
    }
};

template <class Object>
class RobotAdapter : public IRobot
{
    Object& m_obj;
public:
    virtual void Move(int dx, int dy)
    {
        m_obj.Move(dx, dy);
    }
    virtual int GetHealth() const
    {
        return m_obj.GetHealth();
    }

    RobotAdapter(Object& obj)
        : IRobot(obj)
        , m_obj(obj)
    {
    }
};
template <class Object>
RobotAdapter<Object> select_adapter(IRobot*, Object& obj);

struct DummyRobot
{
    void Move(int dx, int dy)
    {
        (void)dx;
        (void)dy;
    }
    int GetHealth() const
    {
        return 10;
    }
    int mana;

    DummyRobot()
        : mana(3)
    {}
};

struct CrazyRobot
{
    void Move(int dx, int dy)
    {
        (void)dx;
        (void)dy;
    }
    int GetHealth() const
    {
        return 1;
    }
    int mana;

    CrazyRobot()
        : mana(1000)
    {}
};


// Interface inheritance

struct IAnimal
{
    virtual int Sleep() = 0;
};
struct IDog : IAnimal
{
    virtual int Play() = 0;
};
struct ICat : IAnimal
{
    virtual int Purr() = 0;
};

template <class Object>
class DogAdapter : public IDog
{
    Object& m_obj;
public:
    virtual int Sleep() { return m_obj.Sleep(); }
    virtual int Play() { return m_obj.Play(); }
    DogAdapter(Object& obj) : m_obj(obj) {}
};
template <class Object>
DogAdapter<Object> select_adapter(IDog*, Object& obj);

template <class Object>
class CatAdapter : public ICat
{
    Object& m_obj;
public:
    virtual int Sleep() { return m_obj.Sleep(); }
    virtual int Purr() { return m_obj.Purr(); }
    CatAdapter(Object& obj) : m_obj(obj) {}
};
template <class Object>
CatAdapter<Object> select_adapter(ICat*, Object& obj);

struct Dog10
{
    int Sleep() { return 1; }
    int Play() { return 10; }
};
struct Dog30
{
    int Sleep() { return 3; }
    int Play() { return 30; }
};
struct Cat20
{
    int Sleep() { return 2; }
    int Purr() { return 20; }
};
struct Cat40
{
    int Sleep() { return 4; }
    int Purr() { return 40; }
};

