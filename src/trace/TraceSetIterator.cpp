/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <babeltrace/ctf/iterator.h>

#include "trace/TraceSetIterator.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee
{
namespace trace
{

TraceSetIterator::TraceSetIterator() :
    _btCtfIter {nullptr},
    _btIter {nullptr}
{
}

TraceSetIterator::TraceSetIterator(bt_context* ctx, const timestamp_t *start, const timestamp_t *finish) :
    _btCtfIter {nullptr},
    _btIter {nullptr}
{
    if (start == nullptr) {
        _btBeginPos.type = ::BT_SEEK_BEGIN;
        _btBeginPos.u.seek_time = 0;
    } else {
        _btBeginPos.type = ::BT_SEEK_TIME;
        _btBeginPos.u.seek_time = *start;
    }

    if (finish == nullptr) {
        _btEndPos.type = ::BT_SEEK_LAST;
        _btEndPos.u.seek_time = 0;
    } else {
        _btEndPos.type = ::BT_SEEK_TIME;
        _btEndPos.u.seek_time = *finish;
    }

    _btCtfIter = ::bt_ctf_iter_create(ctx, &_btBeginPos, &_btEndPos);

    _btIter = ::bt_ctf_get_iter(_btCtfIter);

    // read current event
    _btEvent = ::bt_ctf_iter_read_event(_btCtfIter);

    // end?
    if (!_btEvent) {
        _btIter = nullptr;
        _btCtfIter = nullptr;
        return;
    }

    // create event
    _event = std::unique_ptr<EventValue> {
        new EventValue {std::addressof(_valueFactory)}
    };

    // update event wrapper
    _event->setPrivateEvent(_btEvent);
}

TraceSetIterator::TraceSetIterator(TraceSetIterator &&it) :
    _btCtfIter(std::move(it._btCtfIter)),
    _btIter(std::move(it._btIter))
{
    it._btCtfIter = nullptr;
    it._btIter = nullptr;
}

TraceSetIterator::~TraceSetIterator()
{
    if (_btCtfIter != nullptr) {
        ::bt_ctf_iter_destroy(_btCtfIter);
    }
}

TraceSetIterator& TraceSetIterator::operator=(TraceSetIterator &&rhs)
{
    if (this != &rhs) {
//        if (_btCtfIter != nullptr) {
//            ::bt_ctf_iter_destroy(_btCtfIter);
//        }
        _btIter = std::move(rhs._btIter);
        rhs._btIter = nullptr;

        _btCtfIter = std::move(rhs._btCtfIter);
        rhs._btCtfIter = nullptr;
    }
    return *this;
}

TraceSetIterator& TraceSetIterator::operator++()
{
    if (!_btIter) {
        // disabled
        return *this;
    }

    if (::bt_iter_next(_btIter) < 0) {
        // disable this iterator
        _btIter = nullptr;
        _btCtfIter = nullptr;
        return *this;
    }

    // read current event
    _btEvent = ::bt_ctf_iter_read_event(_btCtfIter);

    // end?
    if (!_btEvent) {
        _btIter = nullptr;
        _btCtfIter = nullptr;
        return *this;
    }

    // reset value factory pools
    _valueFactory.resetPools();

    // update event wrapper
    _event->setPrivateEvent(_btEvent);

    return *this;
}

bool TraceSetIterator::operator==(const TraceSetIterator& rhs)
{
    return _btIter == rhs._btIter;
}

bool TraceSetIterator::operator!=(const TraceSetIterator& rhs)
{
    return !(*this == rhs);
}

const EventValue& TraceSetIterator::operator*() const
{
    /* Behaviour is undefined (could crash) when we're at the end (should
     * be checked first by comparing to and end trace set iterator).
     */

    return *_event;
}

}
}
