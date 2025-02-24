set(CMAKE_LEGACY_CYGWIN_WIN32 0)
declare_cmake_min_version()

project(MorphAllLanguages)

if  ( DEFINED DICTS_FOLDER)
   set( output_morph_folder  ${DICTS_FOLDER}/Morph )
else()
    set( output_morph_folder  ${PROJECT_SOURCE_DIR}/../../Dicts/Morph )
endif()

message("dicts_folder=${output_morph_folder}")

macro (Runmorph_gen langua)
    set( input_folder  ${PROJECT_SOURCE_DIR}/${langua} )
    set( output_folder  ${output_morph_folder}/${langua} )
    make_directory(${output_folder})

    add_custom_target(
        "morph_gen_on_${langua}"
         SOURCES ${output_folder}/morph.bases ${output_folder}/morph.annot ${output_folder}/morph.forms_autom ${output_folder}/gramtab.json
    )

    add_custom_command(
            OUTPUT ${output_folder}/gramtab.json PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${input_folder}/gramtab.json ${output_folder}/gramtab.json
            VERBATIM
    )
    add_custom_command(
        OUTPUT ${output_folder}/morph.bases ${output_folder}/morph.annot ${output_folder}/morph.forms_autom
        COMMAND morph_gen --input project.mwz --output-folder ${output_folder} --postfix-len 5 --min-freq 3
        DEPENDS morph_gen ${input_folder}/morphs.json ${input_folder}/project.mwz ${input_folder}/gramtab.json
        COMMENT "Convert Morphology ${langua}"                 
        WORKING_DIRECTORY ${input_folder}
        VERBATIM
    )
endmacro()


macro (StatDatBinRun langua Topic)
    set (src ${PROJECT_SOURCE_DIR}/${langua}/StatData.txt)
    set (morph_folder ${output_morph_folder}/${langua})
    set (trg ${morph_folder}/${Topic}homoweight.bin)
    add_custom_command(
        OUTPUT ${trg}
        COMMAND StatDatBin --morph-folder ${morph_folder} --input ${src} --language ${langua} --output ${trg}
        DEPENDS StatDatBin ${src} "${morph_folder}/morph.bases" "${morph_folder}/gramtab.json"
        COMMENT "StatDatBin"
        VERBATIM
    )
endmacro()

macro (word_freq_binRun langua Topic)
    set (src ${PROJECT_SOURCE_DIR}/${langua}/WordData.txt)
    set (morph_folder ${output_morph_folder}/${langua})
    set (trg ${morph_folder}/${Topic}wordweight.bin)
    add_custom_command(
        OUTPUT ${trg}
        COMMAND word_freq_bin --morph-folder ${morph_folder} --input ${src} --language ${langua} --output ${trg}
        DEPENDS word_freq_bin ${src} "${morph_folder}/morph.bases" "${morph_folder}/gramtab.json"
        COMMENT "word_freq_bin"
        VERBATIM
    )
endmacro()


macro (CreateAllMorphBinFiles langua)
    Runmorph_gen (${langua})
    
    foreach (Topic l c f)
        StatDatBinRun ( ${langua} ${Topic} ) 
        word_freq_binRun ( ${langua} ${Topic} )
    endforeach ()

    set( folder  ${output_morph_folder}/${langua} )
    add_custom_target(
        "${langua}_Morph"
        SOURCES ${folder}/morph.bases ${folder}/chomoweight.bin ${folder}/lhomoweight.bin ${folder}/fhomoweight.bin ${folder}/cwordweight.bin ${folder}/lwordweight.bin ${folder}/fwordweight.bin
        WORKING_DIRECTORY ${folder}
    )
endmacro()

CreateAllMorphBinFiles(Russian)
CreateAllMorphBinFiles(German)
CreateAllMorphBinFiles(English)

add_custom_target(
    MorphDicts
    DEPENDS  Russian_Morph German_Morph English_Morph
)

