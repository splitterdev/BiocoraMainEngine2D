#pragma once

class WeaponCalibration {
    private:
    WeaponCalibration( int inputCount ) {
        _inputCount = inputCount;
        _firstLayer = NULL;
        _lastLayer = NULL;
        _input = ( double* ) calloc( sizeof( double ), inputCount );
    }
    public:

    class NodeLink {
        public:
        NodeLink() {
            _inputPointer = NULL;
            _weight = ( MathUtils::random() * 1.0 + 3.0 ) * ( rand() & 0x01 ? 1.0 : - 1.0 );
        }
        virtual ~NodeLink() {
        }

        inline double* getInput() {
            return _inputPointer;
        }

        inline void setInput( double* inputPointer ) {
            _inputPointer = inputPointer;
        }

        inline double getWeight() {
            return _weight;
        }

        inline void setWeight( double weight ) {
            _weight = weight;
        }

        private:
        double* _inputPointer;
        double _weight;
    };


    class Node {
        public:
        Node() {
        }
        virtual ~Node() {
        }

        void calculate() {
            double u = 0.0;
            for ( list < NodeLink >::iterator i = _inputs.begin(); i != _inputs.end(); i++ ) {
                NodeLink link = ( *i );
                double* input = link.getInput();
                if ( input ) {
                    u += ( *input ) * link.getWeight();
                }
            }
            _output = sigmoid( u );
        }

        double* getOutput() {
            return &_output;
        }

        double getOutputValue() {
            return _output;
        }

        void clearInputList() {
            _inputs.clear();
        }

        void addInput( NodeLink link ) {
            _inputs.push_back( link );
        }

        inline list < NodeLink > getInputs() {
            return _inputs;
        }

        inline static double sigmoid( double x ) {
            return 1.0 / ( 1.0 + exp( - x ) );
        }

        private:

        list < NodeLink > _inputs;
        double _output;

    };

    class NodeLayer {
        private:
        NodeLayer( int nodeCount ) {
            for ( int i = 0; i < nodeCount; i++ ) {
                Node* node = new Node();
                _nodes.push_back( node );
            }
        }
        public:
        virtual ~NodeLayer() {
            for ( vector < Node* >::iterator i = _nodes.begin(); i != _nodes.end(); i++ ) {
                delete ( *i );
            }
        }

        static NodeLayer* createLayer( int nodeCount ) {
            if ( nodeCount > 0 ) {
                return new NodeLayer( nodeCount );
            }
            return NULL;
        }

        Node* getNode( int nodeId ) {
            if ( ( nodeId >= 0 ) && ( nodeId < int( _nodes.size() ) ) ) {
                return _nodes[ nodeId ];
            }
            return NULL;
        }

        void calculate() {
            for ( vector < Node* >::iterator i = _nodes.begin(); i != _nodes.end(); i++ ) {
                ( *i ) -> calculate();
            }
        }

        inline vector < Node* > getNodes() {
            return _nodes;
        }

        private:
        vector < Node* > _nodes;
    };


    virtual ~WeaponCalibration() {
        if ( _input ) {
            free( _input );
        }
        for ( vector < NodeLayer* >::iterator i = _layers.begin(); i != _layers.end(); i++ ) {
            NodeLayer* layer = ( *i );
            if ( layer ) {
                delete layer;
            }
        }
    }

    static WeaponCalibration* create( int inputCount, vector < int > layerNodeCount ) {
        if ( inputCount < 0 ) {
            return NULL;
        }
        WeaponCalibration* calibration = new WeaponCalibration( inputCount );
        for ( vector < int >::iterator i = layerNodeCount.begin(); i != layerNodeCount.end(); i++ ) {
            NodeLayer* layer = NodeLayer::createLayer( *i );
            if ( layer ) {
                calibration -> _addLayer( layer );
                if ( calibration -> _firstLayer == NULL ) {
                    calibration -> _firstLayer = layer;
                }
                if ( calibration -> _lastLayer ) {
                    vector < Node* > layerNodes = layer -> getNodes();
                    vector < Node* > lastNodes = calibration -> _lastLayer -> getNodes();
                    for ( vector < Node* >::iterator i = layerNodes.begin(); i != layerNodes.end(); i++ ) {
                        for ( vector < Node* >::iterator n = lastNodes.begin(); n != lastNodes.end(); n++ ) {
                            linkNodes( ( *n ), ( *i ) );
                        }
                    }
                }
                calibration -> _lastLayer = layer;
            }
        }
        for ( int i = 0; i < calibration -> _inputCount; i++ ) {
            vector < Node* > nodes = calibration -> _firstLayer -> getNodes();
            for ( vector < Node* >::iterator n = nodes.begin(); n != nodes.end(); n++ ) {
                linkInput( &( calibration -> _input[ i ] ), ( *n ) );
            }
        }
        return calibration;
    }

    double* getInputPointer( int inputId ) {
        if ( ( inputId >= 0 ) && ( inputId < _inputCount ) ) {
            return &_input[ inputId ];
        }
        return NULL;
    }

    void calculate() {
        for ( vector < NodeLayer* >::iterator i = _layers.begin(); i != _layers.end(); i++ ) {
            ( *i ) -> calculate();
        }
    }

    bool calculateFor( vector < double > inputs ) {
        if ( int( inputs.size() ) != _inputCount ) {
            return false;
        }
        double* _inputBackup = ( double* ) calloc( sizeof( double ), _inputCount );
        for ( int i = 0; i < _inputCount; i++ ) {
            _inputBackup[ i ] = _input[ i ];
            _input[ i ] = inputs[ i ];
        }
        calculate();
        for ( int i = 0; i < _inputCount; i++ ) {
            _input[ i ] = _inputBackup[ i ];
        }
        free( _inputBackup );
        return true;
    }

    double* getOutputPointer( int outputId ) {
        if ( _lastLayer ) {
            Node* node = _lastLayer -> getNode( outputId );
            if ( node ) {
                return node -> getOutput();
            }
        }
        return NULL;
    }

    vector < double > getOutputs() {
        vector < double > outputs;
        if ( _lastLayer ) {
            vector < Node* > nodes = _lastLayer -> getNodes();
            for ( vector < Node* >::iterator i = nodes.begin(); i != nodes.end(); i++ ) {
                outputs.push_back( ( *i ) -> getOutputValue() );
            }
        }
        return outputs;
    }

    /*void generateWeights() {
        NodeLayer* layer = ( *k );
        vector < Node* > nodes = layer -> getNodes();
        for ( vector < Node* >::iterator i = nodes.begin(); i != nodes.end(); i++ ) {
            outputs.push_back( ( *i ) -> getOutputValue() );
        }
    }*/

    static void linkNodes( Node* from, Node* to ) {
        if ( from ) {
            linkInput( from -> getOutput(), to );
        }
    }

    static void linkInput( double* input, Node* to ) {
        if ( to ) {
            list < NodeLink > inputs = to -> getInputs();
            for ( list < NodeLink >::iterator i = inputs.begin(); i != inputs.end(); i++ ) {
                NodeLink link = ( *i );
                if ( link.getInput() == input ) {
                    return;
                }
            }
            NodeLink addedLink;
            addedLink.setInput( input );
            to -> addInput( addedLink );
        }
    }

    private:

    inline void _addLayer( NodeLayer* layer ) {
        _layers.push_back( layer );
    }

    int _inputCount;
    double* _input;

    vector < NodeLayer* > _layers;
    NodeLayer* _firstLayer;
    NodeLayer* _lastLayer;

};


