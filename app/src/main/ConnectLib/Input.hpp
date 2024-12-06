//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_INPUT_HPP
#define CONNECT_INPUT_HPP

class Application;

class Input {

public:
    static void init();
    static void free();
    static void onFocusGained();
    static void onFocusLost();

};

#endif //CONNECT_INPUT_HPP
