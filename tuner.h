#ifndef TUNER_H
#define TUNER_H
#include "evolution.h"

using namespace Evolution;

namespace Napoleon
{
    class Board;
    class Encoder;
    class Tuner
    {
        public:
            void Tune();
        private:
            int fitness(Evolution::Chromosome, Board&, Encoder&);
            void load_file();

    };
}

#endif
