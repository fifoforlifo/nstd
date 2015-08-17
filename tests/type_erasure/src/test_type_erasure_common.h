#pragma once

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
private:
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
