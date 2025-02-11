/*
 * Copyright (C) 2015 Canon Inc.
 * Copyright (C) 2015 Igalia.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


// @conditional=ENABLE(READABLE_STREAM_API) || ENABLE(WRITABLE_STREAM_API)
// @internal

function shieldingPromiseResolve(result)
{
    const promise = @Promise.@resolve(result);
    if (promise.@then === @undefined)
        promise.@then = @Promise.prototype.@then;
    return promise;
}

function promiseInvokeOrNoopNoCatch(object, key, args)
{
    "use strict";

    const method = object[key];
    if (method === @undefined)
        return @Promise.@resolve();
    return @shieldingPromiseResolve(method.@apply(object, args));
}

function promiseInvokeOrNoop(object, key, args)
{
    "use strict";

    try {
        return @promiseInvokeOrNoopNoCatch(object, key, args);
    }
    catch(error) {
        return @Promise.@reject(error);
    }

}

function promiseInvokeOrFallbackOrNoop(object, key1, args1, key2, args2)
{
    "use strict";

    try {
        const method = object[key1];
        if (method === @undefined)
            return @promiseInvokeOrNoopNoCatch(object, key2, args2);
        return @shieldingPromiseResolve(method.@apply(object, args1));
    }
    catch(error) {
        return @Promise.@reject(error);
    }
}

function validateAndNormalizeQueuingStrategy(size, highWaterMark)
{
    "use strict";

    if (size !== @undefined && typeof size !== "function")
        throw new @TypeError("size parameter must be a function");

    const normalizedStrategy = { };

    normalizedStrategy.size = size;
    normalizedStrategy.highWaterMark = @Number(highWaterMark);

    if (@isNaN(normalizedStrategy.highWaterMark) || normalizedStrategy.highWaterMark < 0)
        throw new @RangeError("highWaterMark value is negative or not a number");

    return normalizedStrategy;
}

function newQueue()
{
    "use strict";

    return { content: [], size: 0 };
}

function dequeueValue(queue)
{
    "use strict";

    const record = queue.content.@shift();
    queue.size -= record.size;
    return record.value;
}

function enqueueValueWithSize(queue, value, size)
{
    "use strict";

    size = @Number(size);
    if (!@isFinite(size) || size < 0)
        throw new @RangeError("size has an incorrect value");
    queue.content.@push({ value: value, size: size });
    queue.size += size;
}

function peekQueueValue(queue)
{
    "use strict";

    @assert(queue.content.length > 0);

    return queue.content[0].value;
}
