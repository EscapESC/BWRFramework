class Deaerator
{
private:
    /* data */
public:
    float inletValve = 100;
    float outletValve = 100;

    float pressure = 0;

    float waterAmount = 0;

    int update(float delta){
        return 0;
    }

    Deaerator(/* args */){}
    ~Deaerator(){}
};