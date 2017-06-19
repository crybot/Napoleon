#include "board.h"

namespace Napoleon
{
    class ParallelInfo 
    {
        public:
            void UpdateInfo(int depth, int alpha, int beta, const Board& board, bool ready)
            {
                this->beta = beta;
                this->depth = depth;
                this->alpha = alpha;
                this->board = board;
                readyToSearch = ready;
            }
            int Alpha()
            {
                return alpha;
            }
            int Beta()
            {
                return beta;
            }
            int Depth()
            {
                return depth;
            }
            Board Position()
            {
                return board;
            }
            bool Ready()
            {
                return readyToSearch;
            }
            void SetReady(bool ready) 
            { 
                readyToSearch = ready; 
            }

        private:
            int alpha;
            int beta;
            int depth;
            Board board;
            bool readyToSearch;
    };
}
