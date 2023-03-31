#pragma once

class GameStateProcessorSchemaBuilder {
    private:
    GameStateProcessorSchemaBuilder() {
        _schema = new GameStateProcessor::Schema();
    }

    public:
    virtual ~GameStateProcessorSchemaBuilder() {
        delete _schema;
    }

    static GameStateProcessorSchemaBuilder* create() {
        return new GameStateProcessorSchemaBuilder();
    }

    GameStateProcessorSchemaBuilder* asSinglePlayerCampaign() {
        _schema -> addRobot( new EnergyHeatSystemRobot() );
        _schema -> addRobot( new TimedTaskRobot() );
        _schema -> addRobot( new ScriptRobot() );
        _schema -> addRobot( new LogicRobot() );
        _schema -> addRobot( new BulletRobot() );
        _schema -> addRobot( new ParticleRobot() );
        _schema -> addRobot( new PickupRobot() );
        _schema -> addRobot( new SpawnRobot() );
        return this;
    }

    GameStateProcessor::Schema* build() {
        GameStateProcessor::Schema* schema = _schema;
        _schema = new GameStateProcessor::Schema();
        return schema;
    }

    private:

    GameStateProcessor::Schema* _schema;

};


