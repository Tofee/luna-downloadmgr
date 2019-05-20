// Copyright (c) 2012-2019 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include <uriparser/Uri.h>

#include <util/UrlRep.h>

#define URI_TEXT_RANGE_TO_STRING(textRange)                               \
    textRange.first ? std::string(textRange.first,                        \
                                  textRange.afterLast - textRange.first)  \
    : std::string()

UrlRep UrlRep::fromUrl(const char* uri)
{
    UrlRep urlRep;

    if (!uri)
        return urlRep;

    UriParserStateA state;
    UriUriA uriA;
    UriQueryListA* queryList;
    int queryCount;

    state.uri = &uriA;

    urlRep.m_query.clear();

    if (uriParseUriA(&state, uri) != URI_SUCCESS)
        return urlRep;

    urlRep.m_scheme = URI_TEXT_RANGE_TO_STRING(uriA.scheme);
    urlRep.m_userInfo = URI_TEXT_RANGE_TO_STRING(uriA.userInfo);
    urlRep.m_host = URI_TEXT_RANGE_TO_STRING(uriA.hostText);
    urlRep.m_port = URI_TEXT_RANGE_TO_STRING(uriA.portText);
    urlRep.m_fragment = URI_TEXT_RANGE_TO_STRING(uriA.fragment);

    UriPathSegmentA* tmpPath = uriA.pathHead;
    while (tmpPath) {
        if (tmpPath == uriA.pathTail) {
            urlRep.m_pathOnly = urlRep.m_path;
        }
        urlRep.m_path += "/";
        urlRep.m_path += URI_TEXT_RANGE_TO_STRING(tmpPath->text);
        tmpPath = tmpPath->next;
    }

    if (uriA.pathTail)
        urlRep.m_resource = URI_TEXT_RANGE_TO_STRING((uriA.pathTail)->text);

    if (uriA.query.first) {
        uriDissectQueryMallocA(&queryList, &queryCount, uriA.query.first, uriA.query.afterLast);

        UriQueryListA* tmpQueryList = queryList;
        while (tmpQueryList) {
            if (tmpQueryList->key) {
                urlRep.m_query[tmpQueryList->key] = tmpQueryList->value ? tmpQueryList->value : std::string();
            }
            tmpQueryList = tmpQueryList->next;
        }

        uriFreeQueryListA(queryList);
    }

    uriFreeUriMembersA(&uriA);

    //TODO: maybe a more stringent check on validity???
    urlRep.m_valid = true;
    return urlRep;
}

/**
 * Parse a uri and return a new UrlRep instance.
 *
 * @param uri The URI to parse - can be empty.
 * @return A new UrlRep instance.
 */
UrlRep UrlRep::fromUrl(const std::string& uri)
{
    return fromUrl(uri.empty() ? NULL : uri.c_str());
}
