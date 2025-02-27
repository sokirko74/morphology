// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#pragma once

#include "agramtab.h"       // main symbols

 enum EnglishPartOfSpeechEnum
 {
	  eNOUN  = 0,
	  eADJ = 1,
	  eVERB = 2,
	  eVBE = 3,
	  eMOD = 4,
	  eNUMERAL = 5,
	  eCONJ  = 6,
	  eINTERJ = 7,
	  ePREP = 8,
	  ePARTICLE = 9,
	  eART = 10,
	  eADV = 11,
	  ePN = 12,
	  eORDNUM = 13,
	  ePRON = 14,
	  ePOSS = 15,
	  ePN_ADJ = 16,
	  ENGLISH_PART_OF_SPEECH_COUNT = 17
};


enum EnglishGrammemsEnum {
		eSingular = 0,
		ePlural = 1,
		eMasculinum = 2,
		eFeminum = 3,
		eAnimative = 4,
		ePerfective = 5,
		eNominative = 6,
		eObjectCase = 7,
		eNarrative = 8,
		eGeographics = 9,
		eProper = 10,
		ePersonalPronoun = 11,
		ePossessive = 12,
		ePredicative = 13,
		eUncountable = 14,
		eReflexivePronoun = 15,
		eDemonstrativePronoun = 16,
		eMass = 17,
		eComparativ  = 18,
		eSupremum = 19,
		eFirstPerson = 20,
		eSecondPerson = 21,
		eThirdPerson = 22,
		ePresentIndef = 23,
		eInfinitive = 24,
		ePastIndef = 25,
		ePastParticiple = 26,
		eGerund = 27,
		eFuturum = 28,
		eConditional = 29,

		eApostropheS = 30,
        eApostrophe = 31,
		eNames = 32,
        eOrganisation = 33,
		EnglishGrammemsCount = 34
 };


class CEngGramTab : public CAgramtab
{
	const static size_t eStartUp = 0x6161; //aa 
	const static size_t eEndUp = 0x7A7B;  //zz + 1
	const static size_t eMaxGrmCount = eEndUp - eStartUp; // // 5911  (5 Кб) 
public:
	CAgramtabLine*  Lines[eMaxGrmCount];
	CEngGramTab();
	~CEngGramTab();

    void LoadFromRegistry() override;
	
	part_of_speech_t GetPartOfSpeechesCount() const;
	const char* GetPartOfSpeechStr(part_of_speech_t i, NamingAlphabet na = naDefault) const;
	const char* GetPartOfSpeechStrLong(part_of_speech_t i) const override;
	grammem_t GetGrammemsCount()  const;
	const char* GetGrammemStr(size_t i, NamingAlphabet na = naDefault) const;
	size_t GetMaxGrmCount() const;
	CAgramtabLine*& GetLine(size_t LineNo) {return Lines[LineNo];}
	const CAgramtabLine* GetLine(size_t LineNo) const {return Lines[LineNo];};
	size_t GramcodeToLineIndex(const char * s ) const { return  (unsigned char) s[0]*0x100+(unsigned char) s[1] - eStartUp;};
	std::string LineIndexToGramcode(uint16_t i)  const
	{ 
		i += eStartUp;
		char res[3];

		res[0] = (i >> 8);
		res[1] = (0xFF & i);
		res[2] = 0;
		return  res;
	};

	const char* GetRegistryString() const 
	{
		return "Software\\Dialing\\Lemmatizer\\English\\Egramtab";
	};

	long GetClauseTypeByName(const char*) const {assert(false); return 0;};
	const char* GetClauseNameByType(long type) const {assert(false); return 0;};
	bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const;
	bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const;
	const size_t GetClauseTypesCount() const 
	{
		return 0;
	};
	
	bool IsStrongClauseRoot(const part_of_speech_mask_t poses) const;

    bool IsMorphNoun (part_of_speech_mask_t poses)  const;
	bool is_morph_adj (part_of_speech_mask_t poses) const;
	bool is_morph_participle (part_of_speech_mask_t poses) const;
	bool is_morph_pronoun (part_of_speech_mask_t poses) const;
	bool is_morph_pronoun_adjective(part_of_speech_mask_t poses) const;
	bool is_left_noun_modifier  (part_of_speech_mask_t poses, grammems_mask_t grammems) const;
	bool is_numeral (part_of_speech_mask_t poses) const;
	bool is_verb_form (part_of_speech_mask_t poses) const;
	bool is_infinitive(part_of_speech_mask_t poses) const;
	bool is_morph_predk(part_of_speech_mask_t poses) const;
	bool is_morph_adv(part_of_speech_mask_t poses) const;
	bool is_morph_personal_pronoun (part_of_speech_mask_t poses, grammems_mask_t grammems) const;

    bool IsSimpleParticle(const std::string& lemma, part_of_speech_mask_t poses) const;
	bool IsSynNoun(part_of_speech_mask_t poses, const std::string& lemma) const;
	bool IsStandardParamAbbr (const char* WordStrUpper) const;
	bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const;
	bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const;
	grammems_mask_t GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const;
	bool PartOfSpeechIsProductive(part_of_speech_t) const;


};
