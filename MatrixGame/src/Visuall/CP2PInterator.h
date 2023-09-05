// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <iterator>

template <class T>
class CP2PInterator {
public:
    CP2PInterator(T **groups, size_t count) : m_groups(groups), m_count(count){};
    ~CP2PInterator() = default;

    class iterator {
        long num = 0;
        T **groups;

    public:
        explicit iterator(long _num, T **_groups) : num(_num), groups(_groups){};

        iterator &operator++() {
            num = num + 1;
            return *this;
        }

        iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(iterator other) const { return num == other.num; };
        bool operator!=(iterator other) const { return !(*this == other); };
        T *operator*() const { return *(groups + num); };
    };
    iterator begin() { return iterator(0, m_groups); };
    iterator end() { return iterator(m_count, m_groups); };

private:
    T **m_groups;
    size_t m_count;
};
