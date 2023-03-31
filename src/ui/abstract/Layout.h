#pragma once

class Layout : public Component {
    public:
    Layout( ComponentProperties properties ) : Component( properties ) {
    }
    virtual ~Layout() {
    }

    virtual void rescaleContent() = 0;

    virtual void onDisplay() {
    }

};


