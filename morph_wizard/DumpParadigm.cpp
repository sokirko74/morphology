#include "DumpParadigm.h"
#include "wizard.h"
#include "../common/util_classes.h"
#include <sstream>

bool CMorphSession::operator==(const CMorphSession& X) const {
    return (m_UserName == X.m_UserName)
        && (m_SessionStart == X.m_SessionStart)
        && (m_LastSessionSave == X.m_LastSessionSave);
};

void CMorphSession::SetEmpty() {
    m_UserName = "";
    m_SessionStart = "";
    m_LastSessionSave = "";
};

bool CMorphSession::IsEmpty() const {
    return m_UserName.empty();
};

void CMorphSession::GetJson(CJsonObject& out) const {
    out.add_string("user", m_UserName);
    out.add_string("start", m_SessionStart);
    out.add_string("last_save", m_LastSessionSave);
};

CMorphSession& CMorphSession::FromJson(const rapidjson::Value& inj) {
    m_UserName = inj["user"].GetString();
    m_SessionStart = inj["start"].GetString();
    m_LastSessionSave = inj["last_save"].GetString();
    return *this;
}

std::string CMorphSession::GetJsonStr() const {
    rapidjson::Document d(rapidjson::kObjectType);
    CJsonObject v(d);
    GetJson(v);
    return v.dump_rapidjson();
}



//=====================================================================

void CDumpParadigm::SetEmpty() {
    m_PrefixesStr = "";
    m_TypeGrammemsStr = "";
    m_AuthorStr = "";
    m_SlfStr = "";
    m_FirstSlfLineNo = -1;
    m_Session.SetEmpty();
};

const char TypeGrmField[] = "$type_grm";
const char PrefixesField[] = "$prefixes";
const char SessionField[] = "$session";

bool CDumpParadigm::ReadFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors) {
    SetEmpty();
    if (feof(fp)) return false;
    char buf[1000];

    bError = false;
    while (fgets(buf, 1000, fp)) {
        line_no++;
        std::string s = convert_from_utf8(buf, m_pWizard->m_Language);
        {
            const char* auth_patt = "//edited by ";
            size_t qq = s.find(auth_patt);
            if (qq != std::string::npos) {
                m_AuthorStr = s.substr(qq + strlen(auth_patt));
                Trim(m_AuthorStr);
            };
        };
        {
            size_t qq = s.find("//");
            if (qq != std::string::npos)

                s.erase(qq);

        };
        Trim(s);
        if (s.empty()) continue;
        if (s.substr(0, 5) == "=====") {
            if (m_SlfStr.empty() && m_TypeGrammemsStr.empty() && m_PrefixesStr.empty() && !bError)
                continue;
            else
                break;
        };

        if (s.substr(0, strlen(PrefixesField)) == PrefixesField) {
            size_t ind = s.find("=");
            if (ind == std::string::npos) {
                Errors += Format("cannot parse %s field at line", PrefixesField, line_no);
                bError = true;
            }
            else {
                m_PrefixesStr = s.substr(ind + 1);
                Trim(m_PrefixesStr);
            };
            continue;
        };

        if (s.substr(0, strlen(TypeGrmField)) == TypeGrmField) {

            size_t ind = s.find("=");
            if (ind == std::string::npos) {
                Errors += Format("cannot parse %s field at line %i", TypeGrmField, line_no);
                bError = true;
            }
            else {
                m_TypeGrammemsStr = s.substr(ind + 1);
                Trim(m_TypeGrammemsStr);
            };
            continue;
        };

        if (s.substr(0, strlen(SessionField)) == SessionField) {

            size_t ind = s.find("=");
            if (ind == std::string::npos) {
                Errors += Format("cannot parse %s field at line %i", SessionField, line_no);
                bError = true;
            }
            else {
                std::string s = s.substr(ind + 1);
                Trim(s);
                rapidjson::Document d;
                auto& js = d.Parse(s.c_str());
                m_Session.FromJson(js);
            };
            continue;
        };

        if (m_FirstSlfLineNo == -1)
            m_FirstSlfLineNo = line_no - 1;
        m_SlfStr += s;
        m_SlfStr += "\r\n";
    };

    return !m_SlfStr.empty();
};

std::string CDumpParadigm::ToStringUtf8() const {
    std::stringstream outp;
    outp << "=====\n";
    if (!m_PrefixesStr.empty())
        outp << PrefixesField << " = " << m_PrefixesStr << "\n";
    if (!m_TypeGrammemsStr.empty())
        outp << TypeGrmField << " = " << m_TypeGrammemsStr << "\n";
    if (!m_Session.IsEmpty())
        outp << SessionField << " = " << m_Session.GetJsonStr() << "\n";
    outp << m_SlfStr;
    return convert_to_utf8(outp.str(), m_pWizard->m_Language);
};

bool CDumpParadigm::ReadNextParadigmFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors) {
    int start_line_no = line_no;

    if (!ReadFromFile(fp, line_no, bError, Errors)) return false;

    if (!m_pWizard->check_common_grammems(m_TypeGrammemsStr)) {
        Errors += Format("cannot process common grammems in the paradigm at line %i \n", start_line_no);
        bError = true;
    };
    if (!m_pWizard->check_prefixes(m_PrefixesStr)) {
        Errors += Format("cannot process prefixes in the paradigm at  line %i \n", start_line_no);
        bError = true;
    };

    return true;

};
