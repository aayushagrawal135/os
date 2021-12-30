#include "pager.hpp"

Pager::Pager() {
    hand = 0;
    numFrames = DEFAULT_FRAMES;
}

void Pager::setInstructionCount(int instructionCount) {
    this->instructionCount = instructionCount;
}

void Pager::setNumFrames(int numFrames) {
    this->numFrames = numFrames;
}

Frame* FIFO::getVictimFrame() {
    Frame* victim = frameTable[hand];
    hand = (hand + 1) % numFrames;
    return victim;
}

Random::Random(Parser* parser) {
    this->parser = parser;
}

Frame* Random::getVictimFrame() {
    hand = parser->myrandom(numFrames);
    return frameTable[hand];
}

Frame* Clock::getVictimFrame() {
    bool frameFound = false;
    Frame* frame;
    while (!frameFound) {
        frame = frameTable[hand];
        if (frame->vpage->referenced == 0)
            frameFound = true;
        else
            frame->vpage->referenced = 0;
        hand = (hand + 1) % numFrames;
    }
    return frame;
}

NRU::NRU() {
    lastInstructionRest = 0;
}

Frame* NRU::getVictimFrame() {
    bool reset = false;
    int minClassIndex = INF;
    Frame *minFrame = nullptr;
    int minHand = hand;

    if (instructionCount - lastInstructionRest >= RESET_PERIOD) {
        reset = true;
        lastInstructionRest = instructionCount;
    }

    for (int i = 0; i < numFrames; ++i) {
        Frame *frame = frameTable[hand];
        int classIndex = 2 * frame->vpage->referenced + frame->vpage->modified;
        if (classIndex < minClassIndex) {
            minClassIndex = classIndex;
            minFrame = frame;
            minHand = hand;
        }
        if (reset)
            frame->vpage->referenced = 0;
        hand = (hand + 1) % numFrames;
    }
    hand = (minHand + 1) % numFrames;
    return minFrame;
}

void Aging::reduceAge(int frameNum) {
    frameTable[frameNum]->age >>= 1;
}

void Aging::incAgeInExchangeOfRefBit(int frameNum) {
    if (frameTable[frameNum]->vpage->referenced == 1) {
        frameTable[frameNum]->age |= (unsigned int)(1 << 31);
        frameTable[frameNum]->vpage->referenced = 0;
    }
}

Frame* Aging::getVictimFrame() {
    Frame *victim = frameTable[hand];
    for (int i = 0; i < numFrames; i++) {
        int pos = (hand + i) % numFrames;
        reduceAge(pos);
        incAgeInExchangeOfRefBit(pos);
        if (frameTable[pos]->age < victim->age)
            victim = frameTable[pos];
    }
    hand = (victim->frameNum + 1) % numFrames;
    return victim;
}

void WorkingSet::refreshInExchangeOfRefBit(int frameNum) {
    frameTable[frameNum]->time = instructionCount;
    frameTable[frameNum]->vpage->referenced = 0;
}

Frame* WorkingSet::getVictimFrame() {
    Frame* victim = frameTable[hand];
    int maxStaleness = 0;
    for (int i = 0; i < numFrames; i++) {
        int pos = (hand + i) % numFrames;
        int staleness = instructionCount - frameTable[pos]->time;
        if (frameTable[pos]->vpage->referenced == 1)
            refreshInExchangeOfRefBit(pos);
        else if (staleness >= RESET_PERIOD) {
            victim = frameTable[pos];
            break;
        }
        else if (staleness > maxStaleness) {
            victim = frameTable[pos];
            maxStaleness = staleness;
        }
    }
    hand = (victim->frameNum + 1) % numFrames;
    return victim;
}

Frame* Pager::getFrame(unsigned long instructionCount) {
    Frame *frame = this->allocateFrameFromFreeList();
    if (frame != nullptr)
        return frame;
    this->setInstructionCount(instructionCount);
    return this->getVictimFrame();
}

void Pager::resetFrame(int frameNum) {
    frameTable[frameNum]->reset();
}

void Pager::addFreeFrame(int frameNum) {
    freeFrames.push(frameTable[frameNum]);
}

Frame* Pager::allocateFrameFromFreeList() {
    if (freeFrames.empty())
        return nullptr;
    Frame* frame = freeFrames.front();
    freeFrames.pop();
    return frame;
}

void Pager::initFrameTable() {
    for (int i = 0; i < numFrames; ++i) {
        Frame* frame = new Frame(i);
        frameTable.push_back(frame);
        freeFrames.push(frame);
    }
}

Pager::~Pager() {
    for (int i = 0; i < numFrames; ++i) {
        free(frameTable[i]);
    }
}

void Pager::printFrameTable() {
    printf("FT: ");
    for (int i = 0; i < numFrames; ++i) {
        if (frameTable[i]->process != nullptr)
            printf("%d:%d", frameTable[i]->process->getPid(), frameTable[i]->vpage->pageNumber);
        else
            printf("*");
        if (i != numFrames-1)
            printf(" ");
    }
    printf("\n");
}
