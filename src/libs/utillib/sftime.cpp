/*-------------------------------------------------------------------------------------------
 * QuickBlocks - Decentralized, useful, and detailed data from Ethereum blockchains
 * Copyright (c) 2018 Great Hill Corporation (http://quickblocks.io)
 *
 * This program is free software: you may redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version. This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General
 * Public License along with this program. If not, see http://www.gnu.org/licenses/.
 *-------------------------------------------------------------------------------------------*/
#include <algorithm>
#include <string>
#include "basetypes.h"
#include "sftime.h"
#include "sfos.h"
#include "conversions.h"

namespace qblocks {

    #define SECS_PER_DAY  ((unsigned int)(24*60*60))
    #define SECS_PER_HOUR    ((unsigned int)(60*60))
    #define SECS_PER_MIN        ((unsigned int)(60))

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime() {
        m_nSeconds = 0xdeadbeef;
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(const SFTime& date) {
        m_nSeconds  = date.m_nSeconds;
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t sec) {
        *this = SFTime(SFDate(year, month, day), SFTimeOfDay(hour, minute, sec));
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(uint32_t year, uint32_t month, uint32_t weekInMonth, uint32_t dayOfWeek,
                   uint32_t hour, uint32_t minute, uint32_t sec) {
        *this = SFTime(SFDate(year, month, weekInMonth, dayOfWeek), SFTimeOfDay(hour, minute, sec));
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(uint32_t days, uint32_t hour, uint32_t minute, uint32_t sec) {
        *this = SFTime(SFDate(days), SFTimeOfDay(hour, minute, sec));
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(const SFDate& datePart, const SFTimeOfDay& timePart) {
        m_nSeconds = (datePart.GetTotalDays() * SECS_PER_DAY) + timePart.GetTotalSeconds();
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(const string_q& dateStr, const string_q& fmtStr) {
        if (dateStr.empty() || fmtStr.empty())
            *this = earliestDate;
        else
            *this = SFTime(SFDate(dateStr, fmtStr), SFTimeOfDay(dateStr, fmtStr));
    }

    //----------------------------------------------------------------------------------------------------
    SFTime::SFTime(const tm& st, bool useDayOfWeek) {
        tm sysTime = st;
        if (!sysTime.tm_year)
            sysTime.tm_year = static_cast<int>(Now().GetYear());
        ASSERT(sysTime.tm_year);

        if (useDayOfWeek) {
            *this = SFTime(SFDate(
                                  (uint32_t)sysTime.tm_year,
                                  (uint32_t)sysTime.tm_mon,
                                  (uint32_t)sysTime.tm_mday,
                                  (uint32_t)(sysTime.tm_wday+1)),
                                  SFTimeOfDay(sysTime));
        } else {
            *this = SFTime(SFDate(sysTime), SFTimeOfDay(sysTime));
        }
    }

    //----------------------------------------------------------------------------------------------------
    SFTime& SFTime::operator=(const SFTime& d) {
        m_nSeconds  = d.m_nSeconds;
        return *this;
    }

    //----------------------------------------------------------------------------------------------------
    // %a    Abbreviated weekday name
    // %A    Full weekday name
    // %b    Abbreviated month name
    // %B    Full month name
    // %d    Day of month as decimal number (01 - 31)
    // %f    Calendar being used "O.S" Old Style (Julian) or "N.S" New Style (Gregorian)
    // %j    Day of year as decimal number (001 - 366)
    // %m    Month as decimal number (01 - 12)
    // %P    AM/PM indicator
    // %p    a/p indicator
    // %U    Week of year as decimal number
    // %w    Weekday as decimal number (1 - 7; Sunday is 1)
    // %x    Date representation for current locale,  namely
    //         Date representation + " " + Time Representation for current locale
    // %y    Year without century, as decimal number (00 - 99)
    // %Y    Year with century, as decimal number
    // %c    Year displayed using C.E.(Current Epoch) / B.C.E (Before Current Epoch) convention e.g. -1023 = 1022 BCE
    //
    // %H    Hours in the day
    // %h    12 Hour format Hours in (00 - 12)
    // %M    Minutes in the hour
    // %S    Seconds in the minute
    // %Q    Seconds so far this day
    // %%    Percent sign
    //
    //
    // may also need to include full windows escape character
    //
    // %%    Percent sign
    //
    // As in the printf function, the # flag may prefix any formatting code.
    // In that case, the meaning of the format code is changed as follows.
    //
    // Format Code Meaning
    // %#x Long date representation, appropriate to current locale, namely
    //       Long Date representation + " " + Time Representation for current locale
    //
    //
    // %#d, %#j, %#m, %#U, %#w, %#y, %#H, %#h, %#M, %#S  Remove leading zeros (if any).
    //----------------------------------------------------------------------------------------------------
    string_q SFTime::Format(const string_q& sFormat) const {
        string_q ret;
        if (IsValid()) {
            char sBuffer[512];
            size_t bSize = sizeof(sBuffer);

            size_t sFmtLength = sFormat.length();
            for (size_t i = 0; i < sFmtLength; i++) {
                char c = sFormat.at(i);
                if (c == '%') {
                    ++i;
                    if (i < sFmtLength) {
                        c = sFormat.at(i);
                        switch (c) {
                            case 'Q': {
                                uint32_t hour = GetHour();
                                uint32_t minute  = GetMinute();
                                uint32_t secs = GetSecond();
                                ret += asString(hour * 60 * 60 + minute * 60 + secs);
                                break;
                            }
                            case 'H': {
                                snprintf(sBuffer, bSize, "%.02d", GetHour());
                                ret += sBuffer;
                                break;
                            }
                            case 'h': {
                                uint32_t h = GetHour();
                                if (h > 12)
                                    h -= 12;
                                if (h == 0)
                                    h = 12;
                                snprintf(sBuffer, bSize, "%.02d", h);
                                ret += sBuffer;
                                break;
                            }
                            case 'M': {
                                snprintf(sBuffer, bSize, "%.02d", GetMinute());
                                ret += sBuffer;
                                break;
                            }
                            case 'P': {
                                ASSERT(getTimePart().IsValid());
                                ret += (GetHour() >= 12 ? "pm" : "am");
                                break;
                            }
                            case 'p': {
                                ASSERT(getTimePart().IsValid());
                                ret += (GetHour() >= 12 ? "p" : "a");
                                break;
                            }
                            case 'S': {
                                snprintf(sBuffer, bSize, "%.02d", GetSecond());
                                ret += sBuffer;
                                break;
                            }
                            case 'd': {
                                snprintf(sBuffer, bSize, "%.02d", GetDay());
                                ret += sBuffer;
                                break;
                            }
                            case 'f': {
                                ret += "Gregorian Calendar";
                                break;
                            }
                            case 'm': {
                                snprintf(sBuffer, bSize, "%.02d", GetMonth());
                                ret += sBuffer;
                                break;
                            }
                            case 'y': {
                                snprintf(sBuffer, bSize, "%.02d", get2Digit(GetYear()));
                                ret += sBuffer;
                                break;
                            }
                            case 'Y': {
                                snprintf(sBuffer, bSize, "%d", GetYear());
                                ret += sBuffer;
                                break;
                            }
                            case '#': {
                                if (i < sFmtLength) {
                                    ++i;
                                    c = sFormat.at(i);
                                    switch (c) {
                                        case 'd': {
                                            snprintf(sBuffer, bSize, "%d", GetDay());
                                            ret += sBuffer;
                                            break;
                                        }
                                        case 'm': {
                                            snprintf(sBuffer, bSize, "%d", GetMonth());
                                            ret += sBuffer;
                                            break;
                                        }
                                        case 'y': {
                                            snprintf(sBuffer, bSize, "%d", get2Digit(GetYear()));
                                            ret += sBuffer;
                                            break;
                                        }
                                        case 'H': {
                                            snprintf(sBuffer, bSize, "%d", GetHour());
                                            ret += sBuffer;
                                            break;
                                        }
                                        case 'h': {
                                            uint32_t h = GetHour();
                                            if (h > 12)
                                                h -= 12;
                                            if (h == 0)
                                                h = 12;
                                            snprintf(sBuffer, bSize, "%d", h);
                                            ret += sBuffer;
                                            break;
                                        }
                                        case 'M': {
                                            snprintf(sBuffer, bSize, "%d", GetMinute());
                                            ret += sBuffer;
                                            break;
                                        }
                                        case 'S': {
                                            snprintf(sBuffer, bSize, "%d", GetSecond());
                                            ret += sBuffer;
                                            break;
                                        }
                                        default: {
                                            ret += c;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                            default: {
                                ret += c;
                                break;
                            }
                        }
                    }
                } else {
                    ret += c;
                }
            }
        }

        return ret;
    }

    //----------------------------------------------------------------------------------------------------
    ostream &operator<<(ostream &os, const SFTime& x) {
        os << x.Format(FMT_JSON);
        return os;
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFTimeOfDay::GetHour() const {
        ASSERT(IsValid());
        return (uint32_t)(m_nSeconds / SECS_PER_HOUR);
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFTimeOfDay::GetMinute() const {
        ASSERT(IsValid());
        uint32_t secsInHours = GetHour() * SECS_PER_HOUR;
        return (uint32_t)((m_nSeconds - secsInHours) / SECS_PER_MIN);
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFTimeOfDay::GetSecond() const {
        ASSERT(IsValid());
        uint32_t secsInHours = GetHour()   * SECS_PER_HOUR;
        uint32_t secsInMins  = GetMinute() * SECS_PER_MIN;
        return (m_nSeconds - secsInHours - secsInMins);
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFTimeOfDay::GetTotalSeconds() const {
        ASSERT(IsValid());
        return m_nSeconds;
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::GetYear() const {
        return getDatePart().GetYear();
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::GetMonth() const {
        return getDatePart().GetMonth();
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::GetDay() const {
        return getDatePart().GetDay();
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::GetHour() const {
        return getTimePart().GetHour();
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::GetMinute() const {
        return getTimePart().GetMinute();
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::GetSecond() const {
        return getTimePart().GetSecond();
    }

    //-------------------------------------------------------------------------
    timestamp_t SFTime::GetTotalSeconds(void) const {
        return m_nSeconds;
    }

    //-------------------------------------------------------------------------
    extern uint32_t getDayOfWeek(const SFTime::SFDate& date);

    //-------------------------------------------------------------------------
    // We only do the test for equality and greater than. We then use these
    // to do all other tests
    bool SFTime::operator==(const SFTime& date) const {
        ASSERT(IsValid());
        return (m_nSeconds == date.m_nSeconds);
    }

    //-------------------------------------------------------------------------
    bool SFTime::operator!=(const SFTime& date) const {
        return !operator==(date);
    }

    //-------------------------------------------------------------------------
    bool SFTime::operator>(const SFTime& date) const {
        return (m_nSeconds > date.m_nSeconds);
    }

    //-------------------------------------------------------------------------
    bool SFTime::operator>=(const SFTime& date) const {
        return operator>(date) || operator==(date);
    }

    //-------------------------------------------------------------------------
    bool SFTime::operator<(const SFTime& date) const {
        return (m_nSeconds < date.m_nSeconds);
    }

    //-------------------------------------------------------------------------
    bool SFTime::operator<=(const SFTime& date) const {
        return operator<(date) || operator==(date);
    }

    //-------------------------------------------------------------------------
    SFTime SFTime::operator+(const uint32_t& ts) const {
        SFTime ret;
        ret.m_nSeconds = m_nSeconds + ts;
        return ret;
    }

    //-------------------------------------------------------------------------
    SFTime SFTime::operator-(const uint32_t& ts) const {
        SFTime ret;
        ret.m_nSeconds = m_nSeconds - ts;
        return ret;
    }

    //-------------------------------------------------------------------------
    int64_t SFTime::operator-(SFTime& date) {
        return m_nSeconds - date.m_nSeconds;
    }

    //-------------------------------------------------------------------------
    bool SFTime::IsValid() const {
        return (m_nSeconds != (int64_t)0xdeadbeef);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate SFTime::getDatePart() const {
        ASSERT(IsValid());
        return SFDate((m_nSeconds / SECS_PER_DAY) - 2000000000L);
    }

    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay SFTime::getTimePart() const {
        if (m_nSeconds == getDatePart().GetTotalDays())
            return SFTimeOfDay(0);  // midnight at start of day

        ASSERT(IsValid());
        ASSERT(m_nSeconds >= getDatePart().GetTotalDays());

        int64_t secs = m_nSeconds;
        int64_t dateSecs = getDatePart().GetTotalDays() * SECS_PER_DAY;
        return SFTimeOfDay(uint32_t(secs-dateSecs));
    }

    //-------------------------------------------------------------------------
    bool SFTime::onTheHour(void) const {
        return (GetMinute() < 5 || GetMinute() > 55);
    }

    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay::SFTimeOfDay() {
        // create an invalid value (would be the next day otherwise!)
        m_nSeconds = SECS_PER_DAY;
    }

    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay::SFTimeOfDay(const SFTimeOfDay& tod) {
        m_nSeconds = tod.m_nSeconds;
    }

    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay::SFTimeOfDay(uint32_t h, uint32_t m, uint32_t s) {
        m_nSeconds = h * SECS_PER_HOUR + m * SECS_PER_MIN + s;
        m_nSeconds = min(SECS_PER_DAY, m_nSeconds);  // make it invalid of overrun
    }

    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay::SFTimeOfDay(uint32_t secs) {
        m_nSeconds = secs;
        m_nSeconds = min(SECS_PER_DAY, m_nSeconds);  // make it invalid of overrun
    }

    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay::SFTimeOfDay(const tm& sysTime) {
        m_nSeconds =
                ((uint32_t)sysTime.tm_hour) * SECS_PER_HOUR +
                (uint32_t)(sysTime.tm_min) * SECS_PER_MIN +
                (uint32_t)(sysTime.tm_sec);
        m_nSeconds = min(SECS_PER_DAY, m_nSeconds);  // make it invalid of overrun
    }

    //-------------------------------------------------------------------------
    #define toLong32u(a) (uint32_t)toLongU((a))

    //-------------------------------------------------------------------------
    //
    // dateStr is a legal date string as defined by fmtStr.
    //
    // fmtStr is a 4 character string where the first 3 indicate the
    // order of the hour/minute/sec and the fourth character indicates the separator.
    // The date string should have a space between the time and meridian (if
    // meridian is not present then assume AM.  If hour > 12 then assume military
    //
    // For example:
    //
    // fmtStr dateStr Result
    //
    // "hms:" "12:10 " 12:10 pm
    // "hms-" "0-10-0" 12:10:00 am
    //
    // This feature allows for the creation of SFTimeOfDay's from a string
    // as entered in a masked edit field or from a parsed report.
    //
    //-------------------------------------------------------------------------
    SFTime::SFTimeOfDay::SFTimeOfDay(const string_q& timeStr, const string_q& fmtStr) {
        m_nSeconds = SECS_PER_DAY;
        if (fmtStr.length() != 4) {
            *this = SFTimeOfDay(12, 0, 0);
            return;
        }

        uint32_t hour = 12;
        uint32_t minute  = 0;
        uint32_t secs = 0;
        bool am   = true;

        char sep = fmtStr[3];

        string_q str = timeStr;  // 12:12:12 am for example

        for (size_t i = 0 ; i < 3 && str.length() > 0 ; i++) {
            switch (fmtStr[i]) {
                case 'h':
                case 'H':
                    hour = toLong32u(nextTokenClear(str, sep));
                    break;

                case 'm':
                case 'M':
                    minute = toLong32u(nextTokenClear(str, sep));
                    break;

                case 's':
                case 'S':
                    secs = toLong32u(nextTokenClear(str, sep));
                    break;
            }
        }

        size_t find = timeStr.find(" ");
        if (find != string::npos) {
            str = toLower(extract(timeStr, find));
            if (contains(str, "p"))
                am = false;
        }

        if (!am && hour < 12)
            hour += 12;

        else if (am && hour == 12)
            hour = 0;

        *this = SFTimeOfDay(hour, minute, secs);
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFDate::GetDay() const {
        ASSERT(IsValid());
        return getDateStruct().m_Day;
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFDate::GetMonth() const {
        ASSERT(IsValid());
        return getDateStruct().m_Month;
    }

    //-------------------------------------------------------------------------
    uint32_t SFTime::SFDate::GetYear() const {
        ASSERT(IsValid());
        return getDateStruct().m_Year;
    }

    //-------------------------------------------------------------------------
    int64_t SFTime::SFDate::GetTotalDays() const {
        ASSERT(IsValid());
        return (int64_t)m_nDays;
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate SFTime::SFDate::operator+(int32_t days) const {
        int64_t res = ((int64_t)m_nDays) - 2000000000L;
        res += days;
        return SFDate(res);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate SFTime::SFDate::operator-(int32_t days) const {
        return SFDate(int64_t(m_nDays - 2000000000L) - days);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate& SFTime::SFDate::operator+=(int32_t days) {
        *this = *this + days;
        return *this;
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate& SFTime::SFDate::operator-=(int32_t days) {
        *this = *this - days;
        return *this;
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(void) {
        m_nDays = (uint64_t)LONG_MIN;
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(const SFDate& date) {
        m_nDays = date.m_nDays;
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(uint32_t y, uint32_t m, uint32_t d) {
        setValues(y, m, d);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(uint32_t y, uint32_t m, uint32_t weekInMonth, uint32_t dayOfWeek) {
        // assume it fails
        m_nDays = (uint64_t)LONG_MIN;

        // assume any year is OK
        if ((m >= 1 && m <= 12) && (weekInMonth >= 1 && weekInMonth <= 4) && (dayOfWeek >= 1 && dayOfWeek <= 7)) {
            SFDate firstDay(y, m, 1);
            ASSERT(firstDay.IsValid());

            uint32_t dow = getDayOfWeek(firstDay);
            uint32_t d = (((dayOfWeek - dow + 7) % 7) + (7*(weekInMonth-1)) + 1);
            setValues(y, m, d);
        }
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(int64_t days) {
        if (days > 2146905911L)  // Largest valid GD N
            m_nDays = (uint64_t)LONG_MIN;
        else
            m_nDays = (uint64_t)(((int64_t)days) + 2000000000L);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(const tm& sysTime) {
        setValues(
                  (uint32_t)sysTime.tm_year,
                  (uint32_t)sysTime.tm_mon,
                  (uint32_t)sysTime.tm_mday);
    }

    //-------------------------------------------------------------------------
    //
    // dateStr is a legal date string as defined by fmtStr.
    //
    // fmtStr is a 5 character string where the first 3 indicate the
    // order of the month/day/year, the fourth character indicates the length
    // of the year, and the fifth character indicates the separator.
    //
    // For example:
    //
    // fmtStr dateStr Result
    //
    // "dmy2/" "20/03/96" 20th March 1996
    // "mdy4-" "03-28-1996" 28th March 1996
    //
    // This feature allows for the creation of SFDate's from a string
    // as entered in a masked edit field or from a parsed report.
    //-------------------------------------------------------------------------
    SFTime::SFDate::SFDate(const string_q& dateStr, const string_q& fmtStr) {
        m_nDays  = (uint64_t)LONG_MIN;
        if (fmtStr.length() != 5) {
            *this = SFDate(Now().GetYear(), Now().GetMonth(), Now().GetDay());
            return;
        }

        uint32_t day   = 1;
        uint32_t month = 1;
        uint32_t year  = Now().GetYear();

        char sep = fmtStr[4];

        string_q str = dateStr;  // 12-10-1921 for example
        for (size_t i = 0 ; i < 3 && str.length() > 0 ; i++) {
            switch (fmtStr[i]) {
                case 'd':
                case 'D':
                    day = toLong32u(nextTokenClear(str, sep));
                    break;
                case 'm':
                case 'M':
                    month = toLong32u(nextTokenClear(str, sep));
                    break;
                case 'y':
                case 'Y':
                {
                    year = toLong32u(nextTokenClear(str, sep));
                    char c = fmtStr.at(3);
                    ASSERT((c == '2') || (c == '4'));
                    if (c == '2' || year < 100) {
                        if (year >= 50)
                            year += 1900;
                        else if (year < 50)
                            year += 2000;
                    }
                }
                    break;
            }
        }

#define CLAMP(_vv, _ll, _hh) min((uint32_t)_hh, max((uint32_t)_ll, (uint32_t)_vv))
        year  = CLAMP(year, earliestDate.GetYear(), latestDate.GetYear());
        month = CLAMP(month, 1, 12);
        day   = CLAMP(day, 1, DaysInMonth(year, month));

        setValues(year, month, day);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate& SFTime::SFDate::operator=(const SFDate& date) {
        m_nDays = date.m_nDays;
        return *this;
    }

    //-------------------------------------------------------------------------
    template<class TYPE>
    TYPE lfloor(TYPE a, TYPE b) {
        ASSERT(b > 0);
        return (a >= 0 ? a/b : (a%b == 0) - 1 - ((TYPE)labs(a))/b);
    }

    //-------------------------------------------------------------------------
    SFTime::SFDate& SFTime::SFDate::setValues(uint32_t y, uint32_t m, uint32_t d) {
        m_nDays = (uint64_t)LONG_MIN;
        if (m >= JANUARY && m <= DECEMBER && d <= DaysInMonth(y, m)) {
            // The following algorithm has been taken from from an article in
            // the March 1993 issue of the Windows / Dos Developers Journal.
            m_nDays = (y-1)
                        * 365
                        + (uint64_t)lfloor((int32_t)(y-1), 4);

            m_nDays += (uint64_t)lfloor((int32_t)(y-1), 400) -
                        (uint64_t)lfloor((int32_t)(y-1), 100);

            --m;
            while (m) {
                m_nDays += DaysInMonth(y, m);
                m--;
            }
            m_nDays += (d + 1999422264L);  // ensure all usable date values are positive by adding 2 billion to m_nDays
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    SFDateStruct SFTime::SFDate::getDateStruct() const {
        // See the comment in SFDate::setValues(uint32_t Year, uint32_t Month, uint32_t Day)
        // for references to where the algorithm is taken from
        ASSERT(IsValid());

        SFDateStruct ds;
        int64_t gdn  = int64_t(m_nDays - 1999422265);
        int64_t y4   = 1461;
        int64_t y400 = 146100 - 3;
        int64_t y100 = 36525 - 1;

        bool exception = false;
        ds.m_Year = (uint32_t)(400 * lfloor(gdn, y400));
        gdn -= y400 * lfloor(gdn, y400);       // 400 year periods

        if (gdn > 0L) {
            ds.m_Year += 100*lfloor(gdn, y100);  // 100 year periods
            gdn -= y100*lfloor(gdn, y100);
            exception = (gdn == 0 && true);
            if (gdn > 0) {
                ds.m_Year += 4*lfloor(gdn, y4);     // 4 year periods
                gdn -= y4 * lfloor(gdn, y4);
                if (gdn > 0) {
                    int i = 0;
                    while (gdn > 365 && ++i < 4) {
                        ds.m_Year++;
                        gdn -= 365;
                    }
                }
            }
        }

        if (exception) {
            gdn = 366;  // occurs once every hundred years with Gregorian calendar

        } else {
            ds.m_Year++;
            gdn++;
        }

        ds.m_Month = 1;
        while (ds.m_Month < 13 && gdn > DaysInMonth(ds.m_Year, ds.m_Month))
            gdn -= DaysInMonth(ds.m_Year, ds.m_Month++);

        if (ds.m_Month == 13) {
            ds.m_Month = 1;
            ds.m_Year++;
        }

        ds.m_Day = (uint32_t)gdn;
        return ds;
    }

    //---------------------------------------------------------------------------------------
    uint32_t getDayOfWeek(const SFTime::SFDate& date) {
        ASSERT(date.IsValid());
        return (date.GetTotalDays() % 7) + 1;
    }

    //---------------------------------------------------------------------------------------
    SFTime parseDate(const string_q& strIn) {
        if (strIn.empty())
            return earliestDate;

        string_q str = strIn;
        replaceAll(str, ";", "");
        if (str.length() != 14) {
            str += "120000";
        }

        uint32_t y  = toLong32u(extract(str,  0, 4));
        uint32_t m  = toLong32u(extract(str,  4, 2));
        uint32_t d  = toLong32u(extract(str,  6, 2));
        uint32_t h  = toLong32u(extract(str,  8, 2));
        uint32_t mn = toLong32u(extract(str, 10, 2));
        uint32_t s  = toLong32u(extract(str, 12, 2));

        return SFTime(y, m, d, h, mn, s);
    }

    //---------------------------------------------------------------------------------------
    SFTime Now(void) {
        time_t loc;
        time(&loc);

        tm unused;
        struct tm *ret = localtime_r(&loc, &unused);
        if (ret) {
            tm sysTime = *ret;
            sysTime.tm_year += 1900;
            sysTime.tm_mon += 1;
            return SFTime(sysTime, false);
        }
        return earliestDate;
    }

    //---------------------------------------------------------------------------------------
    const SFTime latestDate = SFTime(2200, 12, 31, 23, 59, 59);
    const SFTime earliestDate = SFTime(1700,  1,  1,  0,  0,  1);

    //---------------------------------------------------------------------------------------
    const uint32_t DaysPerMonth[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    //---------------------------------------------------------------------------------------
    bool isLeap(uint32_t year) {
        if ((year % 400) == 0)  // centuries every 400 years are leaps
            return true;

        if (((year % 4) == 0) && ((year % 100) != 0))  // otherwise every four years (but not centuries)
            return true;

        return false;
    }

    //---------------------------------------------------------------------------------------
    uint32_t DaysInMonth(uint32_t year, uint32_t month) {
        if (month == 2 && isLeap(year))
            return 29;
        return DaysPerMonth[month-1];
    }

    //---------------------------------------------------------------------------------------
    uint32_t DaysInYear(uint32_t year) {
        return isLeap(year) ? 366 : 365;
    }

    //---------------------------------------------------------------------------------------
    uint32_t DaysInYear(const SFTime& date) {
        return DaysInYear(date.GetYear());
    }

    //---------------------------------------------------------------------------------------
    SFTime AddOneDay(const SFTime& date) {
        if (date >= latestDate)
            return latestDate;

        uint32_t day    = date.GetDay();
        uint32_t month  = date.GetMonth();
        uint32_t year   = date.GetYear();
        uint32_t minute = date.GetMinute();
        uint32_t sec    = date.GetSecond();
        uint32_t hour   = date.GetHour();
        uint32_t nDays  = DaysInMonth(year, month);

        if (nDays == day) {
            if (month == 12) {
                month = 1;
                year  += 1;
                return SFTime(year, month, 1, hour, minute, sec);
            } else {
                month += 1;
                return SFTime(year, month, 1, hour, minute, sec);
            }
        }

        return SFTime(year, month, day+1, hour, minute, sec);
    }

    //---------------------------------------------------------------------------------------
    SFTime SubtractOneDay(const SFTime& date) {
        if (date <= earliestDate)
            return earliestDate;

        uint32_t day    = date.GetDay();
        uint32_t month  = date.GetMonth();
        uint32_t year   = date.GetYear();
        uint32_t minute = date.GetMinute();
        uint32_t sec    = date.GetSecond();
        uint32_t hour   = date.GetHour();

        if (day == 1) {
            if (month == 1) {
                month = 12;
                year  -= 1;
                return SFTime(year, month, DaysInMonth(year, month), hour, minute, sec);
            } else {
                month -= 1;
                return SFTime(year, month, DaysInMonth(year, month), hour, minute, sec);
            }
        }

        return SFTime(year, month, --day, hour, minute, sec);
    }

    //----------------------------------------------------------------------------------------------------
    SFTime BOW(const SFTime& tm) {
        SFTime ret = BOD(tm);
        while (getDayOfWeek(ret.getDatePart()) > 1)  // if it equals '1', it's Sunday at 00:00:01
            ret = SubtractOneDay(ret);
        return ret;
    }

    //----------------------------------------------------------------------------------------------------
    SFTime EOW(const SFTime& tm) {
        SFTime ret = EOD(tm);
        while (getDayOfWeek(tm.getDatePart()) < 7)  // if it equals '7', it's Saturday 12:59:59
            ret = AddOneDay(ret);
        return ret;
    }

    //----------------------------------------------------------------------------------------------------
    timestamp_t toTimestamp(const SFTime& timeIn) {
        SFTime  jan1970(1970, 1, 1, 0, 0, 0);
        if (timeIn < jan1970)
            return 0;

        int64_t j70 = jan1970.GetTotalSeconds();
        int64_t t   = timeIn.GetTotalSeconds();
        return (t - j70);
    }

    //----------------------------------------------------------------------------------------------------
    SFTime dateFromTimeStamp(timestamp_t tsIn) {
        time_t utc = tsIn;
        tm unused;
        struct tm *ret = gmtime_r(&utc, &unused);

        char retStr[40];
        strftime(retStr, sizeof(retStr), "%Y-%m-%d %H:%M:%S UTC", ret);

        string_q str = retStr;
        uint32_t y = toLong32u(nextTokenClear(str, '-'));
        uint32_t m = toLong32u(nextTokenClear(str, '-'));
        uint32_t d = toLong32u(nextTokenClear(str, ' '));
        uint32_t h = toLong32u(nextTokenClear(str, ':'));
        uint32_t mn = toLong32u(nextTokenClear(str, ':'));
        uint32_t s = toLong32u(nextTokenClear(str, ' '));
        return SFTime(y, m, d, h, mn, s);
    }

    //----------------------------------------------------------------------------------------------------
    string_q fromTimestamp(timestamp_t ts) {
        return asString(ts);
    }

    //----------------------------------------------------------------------------------------------------
    timestamp_t toTimestamp(const string_q& timeIn) {
        return (startsWith(timeIn, "0x") ? (timestamp_t)hex2LongU(timeIn) : (timestamp_t)toLong(timeIn));
    }

    //----------------------------------------------------------------------------------------------------
    SFTime fileLastModifyDate(const string_q& filename) {
        if (!fileExists(filename))
            return earliestDate;

        struct stat statBuf;
        stat(filename.c_str(), &statBuf);

        // Convert time_t to struct tm
        tm unused;
        tm *t = localtime_r(&statBuf.st_mtime, &unused);
        ASSERT(t);
        return SFTime(  (uint32_t)t->tm_year + 1900,
                        (uint32_t)t->tm_mon+1,
                        (uint32_t)t->tm_mday,
                        (uint32_t)t->tm_hour,
                        (uint32_t)t->tm_min,
                        (uint32_t)t->tm_sec);
    }

}  // namespace qblocks
