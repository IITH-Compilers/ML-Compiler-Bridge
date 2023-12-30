function(tf_get_absolute_path path base final_path)
  if (IS_ABSOLUTE ${path})
    set(${final_path} ${path} PARENT_SCOPE)
  else()
    set(${final_path} ${base}/${path} PARENT_SCOPE)
  endif()
endfunction()

function(tf_get_model model final_path)
  string(FIND ${model} "http:" pos_http)
  string(FIND ${model} "https:" pos_https)
  if (${pos_http} EQUAL 0 OR ${pos_https} EQUAL 0)
    message("Downloading model " ${model})
    string(FIND ${model} "/" fname_start REVERSE)
    math(EXPR fname_start "${fname_start}+1")
    string(SUBSTRING ${model} ${fname_start}+1 -1 fname)
    message("Model archive: " ${fname})
    file(DOWNLOAD ${model} ${CMAKE_CURRENT_BINARY_DIR}/${fname})
    file(ARCHIVE_EXTRACT INPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${fname}
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${fname}_model)
    set(${final_path} ${CMAKE_CURRENT_BINARY_DIR}/${fname}_model/model PARENT_SCOPE)
  else()
    tf_get_absolute_path(${model} ${CMAKE_CURRENT_BINARY_DIR} model_path)
    set(${final_path} ${model_path} PARENT_SCOPE)
  endif()
endfunction()

# Generate a mock model for tests.
function(generate_mock_model generator output)
  tf_get_absolute_path(${generator} ${CMAKE_CURRENT_SOURCE_DIR} generator_absolute_path)
  tf_get_absolute_path(${output} ${CMAKE_CURRENT_BINARY_DIR} output_absolute_path)
  message(WARNING "Autogenerated mock models should not be used in production builds.")
  execute_process(COMMAND ${Python3_EXECUTABLE}
    ${generator_absolute_path}
    ${output_absolute_path}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endfunction()

# Run the tensorflow compiler (saved_model_cli) on the saved model in the
# ${model} directory, looking for the ${tag_set} tag set, and the SignatureDef
# ${signature_def_key}.
# Produce a pair of files called ${fname}.h and  ${fname}.o in the
# ${CMAKE_CURRENT_BINARY_DIR}. The generated header will define a C++ class
# called ${cpp_class} - which may be a namespace-qualified class name.
function(tf_compile model tag_set signature_def_key fname cpp_class hdr_file obj_file)
  tf_get_absolute_path(${model} ${CMAKE_CURRENT_BINARY_DIR} LLVM_ML_MODELS_ABSOLUTE)
  add_custom_command(OUTPUT ${obj_file} ${hdr_file}
    COMMAND ${TENSORFLOW_AOT_COMPILER} aot_compile_cpu
          --multithreading false
          --dir ${LLVM_ML_MODELS_ABSOLUTE}
          --tag_set ${tag_set}
          --signature_def_key ${signature_def_key}
          --output_prefix ${prefix}
          --cpp_class ${cpp_class}
          --target_triple ${LLVM_HOST_TRIPLE}
  )

  # Aggregate the objects so that results of different tf_compile calls may be
  # grouped into one target.
  set(GENERATED_OBJS ${GENERATED_OBJS} ${obj_file} PARENT_SCOPE)
  set_source_files_properties(${obj_file} PROPERTIES
    GENERATED 1 EXTERNAL_OBJECT 1)

  set(GENERATED_HEADERS ${GENERATED_HEADERS} ${hdr_file} PARENT_SCOPE)
  set_source_files_properties(${hdr_file} PROPERTIES
    GENERATED 1)

  endfunction()

function(tf_find_and_compile model default_url default_path test_model_generator tag_set signature_def_key fname cpp_class opath)
  set(prefix ${opath}/${fname})
  set(obj_file ${prefix}.o)
  set(hdr_file ${prefix}.h)
  string(TOUPPER ${fname} fname_allcaps)
  set(override_header ${LLVM_OVERRIDE_MODEL_HEADER_${fname_allcaps}})
  set(override_object ${LLVM_OVERRIDE_MODEL_OBJECT_${fname_allcaps}})
  # If the user specified overrides, that indicates intent to use AOT and we
  # don't care what the model path is
  if (EXISTS "${override_header}" AND EXISTS "${override_object}")
    configure_file(${override_header} ${hdr_file} COPYONLY)
    configure_file(${override_object} ${obj_file} COPYONLY)
    message(STATUS "Using provided header " ${hdr_file} " and object " ${obj_file} "
      files for model " ${fname})  
    set(GENERATED_OBJS ${GENERATED_OBJS} ${obj_file})
    set(GENERATED_HEADERS ${GENERATED_HEADERS} ${hdr_file})
  elseif("${model}" STREQUAL "none")
    message(STATUS "Will skip enabling mlgo for ${fname}")
    return()
  else()
    if ("${model}" STREQUAL "download")
      # Crash if the user wants to download a model but a URL is set to "TO_BE_UPDATED"
      if ("${default_url}" STREQUAL "<UNSPECIFIED>")
          message(FATAL_ERROR "Model path was set to 'download' but there is no"
          " model url currently specified in cmake. You can generate a model"
          " using, for example, the tools at http://github.com/google/ml-compiler-opt."
          " Some reference models are also periodically released there.")
      endif()

      set(model ${default_url})
    endif()

    if ("${model}" STREQUAL "autogenerate")
      set(model ${default_path}-autogenerated)
      generate_mock_model(${test_model_generator} ${model})
    endif()

    tf_get_model(${model} LLVM_ML_MODELS_ABSOLUTE)
    tf_compile(${LLVM_ML_MODELS_ABSOLUTE} ${tag_set} ${signature_def_key} ${fname} ${cpp_class} ${hdr_file} ${obj_file})
  endif()

  set(GeneratedMLSources ${GeneratedMLSources} ${GENERATED_OBJS} ${GENERATED_HEADERS} PARENT_SCOPE)
  set(MLDeps ${MLDeps} tf_xla_runtime PARENT_SCOPE)
  set(MLLinkDeps ${MLLinkDeps} tf_xla_runtime PARENT_SCOPE)
  add_compile_definitions(LLVM_HAVE_TF_AOT_${fname_allcaps})
endfunction()
