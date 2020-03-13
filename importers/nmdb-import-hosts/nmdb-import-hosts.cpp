// =============================================================================
// Copyright 2017 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// =============================================================================
// Maintained by Sandia National Laboratories <Netmeld@sandia.gov>
// =============================================================================

#include <netmeld/core/objects/IpAddress.hpp>
#include <netmeld/core/parsers/ParserDomainName.hpp>
#include <netmeld/core/parsers/ParserIpAddress.hpp>
#include <netmeld/core/tools/AbstractImportTool.hpp>

namespace nmco = netmeld::core::objects;
namespace nmcp = netmeld::core::parsers;
namespace nmct = netmeld::core::tools;

typedef std::vector<nmco::IpAddress>      Result;


class Parser :
  public qi::grammar<nmcp::IstreamIter, Result(), qi::ascii::blank_type>
{
  public:
    Parser() : Parser::base_type(start)
    {
      start =
        *(line)
        ;

      line =
        (ipAddr
           [qi::_val = qi::_1] >>
         +hostname
           [pnx::bind(&nmco::IpAddress::addAlias,
                      &qi::_val, qi::_1, "hosts file")] >>
         -comment >> qi::eol
        )
        | (comment >> qi::eol)
        | (qi::eol)
        ;

      comment
        = (qi::lit('#') >> *(qi::ascii::graph | qi::ascii::blank))
        ;

      BOOST_SPIRIT_DEBUG_NODES(
          (start)
          (line)
          (comment)
          );
    }

    qi::rule<nmcp::IstreamIter, Result(), qi::ascii::blank_type>
      start;

    qi::rule<nmcp::IstreamIter, nmco::IpAddress(), qi::ascii::blank_type>
      line;

    qi::rule<nmcp::IstreamIter, qi::ascii::blank_type>
      comment;

    nmcp::ParserIpAddress
      ipAddr;

    nmcp::ParserDomainName
      hostname;
};

template<typename P, typename R>
class Tool : public nmct::AbstractImportTool<P,R>
{
  public:
    Tool() : nmct::AbstractImportTool<P,R>
      ("/etc/hosts", PROGRAM_NAME, PROGRAM_VERSION)
    {}

    void
    specificInserts(pqxx::transaction_base& t) override
    {
      auto toolRunId {this->getToolRunId()};
      auto deviceId  {this->getDeviceId()};
      auto results   {this->tResults};

      for (auto& result : results) {
        result.save(t, toolRunId, deviceId);
        LOG_DEBUG << result.toDebugString() << std::endl;
      }
    }
};


int
main(int argc, char** argv)
{
  Tool<Parser, Result> tool;
  return tool.start(argc, argv);
}
