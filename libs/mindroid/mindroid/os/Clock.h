/*
 * Copyright (C) 2011 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDROID_CLOCK_H_
#define MINDROID_CLOCK_H_

#include <stdint.h>
#include <utils/Timers.h>
#include <mindroid/util/Utils.h>

#define CLOCK_REALTIME  0
#define CLOCK_MONOTONIC 1

namespace mindroid {

class Clock
{
public:
	static uint64_t monotonicTime(); // nanoseconds
	static uint64_t realTime(); // nanoseconds

private:
	NO_COPY_CTOR_AND_ASSIGNMENT_OPERATOR(Clock)
};

} /* namespace mindroid */

#endif /* MINDROID_CLOCK_H_ */
