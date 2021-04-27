#include "channel.h"

MidiChannel::MidiChannel(MidiStream & _ms, bool _drum) :
    ms (_ms)
{
    if (!ms.getChannel(channel, _drum)) throw No_Channel{};
}