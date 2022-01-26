// Copyright (c) 2005-2021 Jay Berkenbilt
//
// This file is part of qpdf.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Versions of qpdf prior to version 7 were released under the terms
// of version 2.0 of the Artistic License. At your option, you may
// continue to consider qpdf to be licensed under those terms. Please
// see the manual for additional information.

#ifndef QPDFJOB_HH
#define QPDFJOB_HH

#include <qpdf/DLL.h>
#include <qpdf/Constants.h>
#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageObjectHelper.hh>

#include <memory>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <functional>
#include <memory>
#include <stdexcept>

class QPDFWriter;

class QPDFJob
{
  public:
    // ConfigError exception is thrown if there are any usage-like
    // errors when calling Config methods.
    class QPDF_DLL_CLASS ConfigError: public std::runtime_error
    {
      public:
        QPDF_DLL
        ConfigError(std::string const&);
    };

    QPDF_DLL
    QPDFJob();

    // SETUP FUNCTIONS

    // Initialize a QPDFJob object from argv. The progname_env
    // argument is the name of an environment variable which, if set,
    // overrides the name of the executable for purposes of generating
    // the --completion options. See QPDFArgParser for details. If a
    // null pointer is passed in, the default value of
    // "QPDF_EXECUTABLE" is used. This is used by the QPDF cli, which
    // just initializes a QPDFJob from argv, calls run(), and handles
    // errors and exit status issues. You can perform much of the cli
    // functionality programmatically in this way rather than using
    // the regular API. This is exposed in the C API, which makes it
    // easier to get certain high-level qpdf functionality from other
    // languages. If there are any command-line errors, this method
    // will throw QPDFArgParser::Usage which is derived from
    // std::runtime_error. Other exceptions may be thrown in some
    // cases. Note that argc, and argv should be UTF-8 encoded. If you
    // are calling this from a Windows Unicode-aware main (wmain), see
    // QUtil::call_main_from_wmain for information about converting
    // arguments to UTF-8. This method will mutate arguments that are
    // passed to it.
    QPDF_DLL
    void initializeFromArgv(int argc, char* argv[],
                            char const* progname_env = nullptr);

    QPDF_DLL
    void initializeFromJson(std::string const& json);

    // Set name that is used to prefix verbose messages, progress
    // messages, and other things that the library writes to output
    // and error streams on the caller's behalf. Defaults to "qpdf".
    QPDF_DLL
    void setMessagePrefix(std::string const&);

    // Override streams that errors and output go to. Defaults are
    // std::cout and std::cerr. Pass nullptr to use the default.
    QPDF_DLL
    void setOutputStreams(std::ostream* out_stream, std::ostream* err_stream);

    // Check to make sure no contradictory options have been
    // specified. This is called automatically after initializing from
    // argv or json and is also called by run, but you can call it
    // manually as well. It throws a Usage exception if there are any
    // errors.
    QPDF_DLL
    void checkConfiguration();

    // Returns true if output is created by the specified job.
    QPDF_DLL
    bool createsOutput() const;

    // SEE BELOW FOR MORE PUBLIC METHODS AND CLASSES
  private:
    // These structures are private but we need to define them before
    // the public Config classes.
    struct CopyAttachmentFrom
    {
        std::string path;
        std::string password;
        std::string prefix;
    };

    struct AddAttachment
    {
        AddAttachment() :
            replace(false)
        {
        }

        std::string path;
        std::string key;
        std::string filename;
        std::string creationdate;
        std::string moddate;
        std::string mimetype;
        std::string description;
        bool replace;
    };

    struct PageSpec
    {
        PageSpec(std::string const& filename,
                 char const* password,
                 std::string const& range);

        std::string filename;
        std::shared_ptr<char> password;
        std::string range;
    };

  public:
    // CONFIGURATION

    // Configuration classes are implemented in QPDFJob_config.cc.

    // The config() method returns a shared pointer to a Config
    // object. The Config object contains methods that correspond with
    // qpdf command-line arguments. You can use a fluent interface to
    // configure a QPDFJob object that would do exactly the same thing
    // as a specific qpdf command. The example pdf-job.cc contains an
    // example of this usage. You can also use initializeFromJson or
    // initializeFromArgv to initialize a QPDFJob object.

    // Notes about the Config methods:
    //
    // * Most of the method declarations are automatically generated
    //   in header files that are included within the class
    //   definitions. They correspond in predictable ways to the
    //   command-line arguments and are generated from the same code
    //   that generates the command-line argument parsing code.
    //
    // * Methods return pointers, rather than references, to
    //   configuration objects. References might feel more familiar to
    //   users of fluent interfaces, so why do we use pointers? The
    //   main methods that create them return smart pointers so that
    //   users can initialize them when needed, which you can't do
    //   with references. Returning pointers instead of references
    //   makes for a more uniform interface.

    class Config;

    class AttConfig
    {
        friend class QPDFJob;
        friend class Config;
      public:
        QPDF_DLL
        Config* endAddAttachment();
        QPDF_DLL
        AttConfig* path(char const* parameter);

#       include <qpdf/auto_job_c_att.hh>

      private:
        AttConfig(Config*);
        AttConfig(AttConfig const&) = delete;

        Config* config;
        AddAttachment att;
    };

    class CopyAttConfig
    {
        friend class QPDFJob;
        friend class Config;
      public:
        QPDF_DLL
        Config* endCopyAttachmentsFrom();
        QPDF_DLL
        CopyAttConfig* path(char const* parameter);

#       include <qpdf/auto_job_c_copy_att.hh>

      private:
        CopyAttConfig(Config*);
        CopyAttConfig(CopyAttConfig const&) = delete;

        Config* config;
        CopyAttachmentFrom caf;
    };

    class PagesConfig
    {
        friend class QPDFJob;
        friend class Config;
      public:
        QPDF_DLL
        Config* endPages();
        QPDF_DLL
        PagesConfig* pageSpec(std::string const& filename,
                              std::string const& range,
                              char const* password = nullptr);

#       include <qpdf/auto_job_c_pages.hh>

      private:
        PagesConfig(Config*);
        PagesConfig(PagesConfig const&) = delete;

        Config* config;
    };

    class UOConfig
    {
        friend class QPDFJob;
        friend class Config;
      public:
        QPDF_DLL
        Config* endUnderlayOverlay();
        QPDF_DLL
        UOConfig* path(char const* parameter);

#       include <qpdf/auto_job_c_uo.hh>

      private:
        UOConfig(Config*);
        UOConfig(PagesConfig const&) = delete;

        Config* config;
    };

    class EncConfig
    {
        friend class QPDFJob;
        friend class Config;
      public:
        QPDF_DLL
        Config* endEncrypt();
        QPDF_DLL
        EncConfig* path(char const* parameter);

#       include <qpdf/auto_job_c_enc.hh>

      private:
        EncConfig(Config*);
        EncConfig(PagesConfig const&) = delete;

        Config* config;
    };

    class Config
    {
        friend class QPDFJob;
      public:
        QPDF_DLL
        Config* inputFile(char const* filename);
        QPDF_DLL
        Config* emptyInput();
        QPDF_DLL
        Config* outputFile(char const* filename);
        QPDF_DLL
        Config* replaceInput();

        QPDF_DLL
        std::shared_ptr<CopyAttConfig> copyAttachmentsFrom();
        QPDF_DLL
        std::shared_ptr<AttConfig> addAttachment();
        QPDF_DLL
        std::shared_ptr<PagesConfig> pages();
        QPDF_DLL
        std::shared_ptr<UOConfig> overlay();
        QPDF_DLL
        std::shared_ptr<UOConfig> underlay();
        QPDF_DLL
        std::shared_ptr<EncConfig> encrypt(
            int keylen,
            std::string const& user_password,
            std::string const& owner_password);

#       include <qpdf/auto_job_c_main.hh>

      private:
        Config() = delete;
        Config(Config const&) = delete;
        Config(QPDFJob& job) :
            o(job)
        {
        }
        QPDFJob& o;
    };
    friend class Config;

    // Return a top-level configuration item. See CONFIGURATION above
    // for details.
    QPDF_DLL
    std::shared_ptr<Config> config();

    // Options for helping the qpdf CLI use the correct edit code and
    // properly report warnings.
    QPDF_DLL
    bool suppressWarnings();
    QPDF_DLL
    bool warningsExitZero();
    QPDF_DLL
    bool checkRequiresPassword();
    QPDF_DLL
    bool checkIsEncrypted();

    // Execute the job
    QPDF_DLL
    void run();

    // CHECK STATUS -- these methods provide information known after
    // run() is called.

    QPDF_DLL
    bool hasWarnings();

    // Return value is bitwise OR of values from qpdf_encryption_status_e
    QPDF_DLL
    unsigned long getEncryptionStatus();

    // HELPER FUNCTIONS -- methods useful for calling in handlers that
    // interact with QPDFJob during run or initialization.

    // If in verbose mode, call the given function, passing in the
    // output stream and message prefix.
    QPDF_DLL
    void doIfVerbose(
        std::function<void(std::ostream&, std::string const& prefix)> fn);

    // Provide a string that is the help information ("schema" for the
    // qpdf-specific JSON object) for version 1 of the JSON output.
    QPDF_DLL
    static std::string json_out_schema_v1();

    // Provide a string that is the help information for the version 1
    // of JSON format for QPDFJob.
    QPDF_DLL
    static std::string json_job_schema_v1();

  private:
    struct RotationSpec
    {
        RotationSpec(int angle = 0, bool relative = false) :
            angle(angle),
            relative(relative)
        {
        }

        int angle;
        bool relative;
    };

    enum password_mode_e { pm_bytes, pm_hex_bytes, pm_unicode, pm_auto };

    struct UnderOverlay
    {
        UnderOverlay(char const* which) :
            which(which),
            to_nr("1-z"),
            from_nr("1-z"),
            repeat_nr("")
        {
        }

        std::string which;
        std::string filename;
        std::shared_ptr<char> password;
        std::string to_nr;
        std::string from_nr;
        std::string repeat_nr;
        std::shared_ptr<QPDF> pdf;
        std::vector<int> to_pagenos;
        std::vector<int> from_pagenos;
        std::vector<int> repeat_pagenos;
    };

    // Helper functions
    static JSON json_schema(std::set<std::string>* keys = 0);
    static void parse_object_id(
        std::string const& objspec, bool& trailer, int& obj, int& gen);
    void parseRotationParameter(std::string const&);
    std::vector<int> parseNumrange(char const* range, int max);

    // Basic file processing
    std::shared_ptr<QPDF> processFile(
        char const* filename, char const* password);
    std::shared_ptr<QPDF> processInputSource(
        PointerHolder<InputSource> is, char const* password);
    std::shared_ptr<QPDF> doProcess(
        std::function<void(QPDF*, char const*)> fn,
        char const* password, bool empty);
    std::shared_ptr<QPDF> doProcessOnce(
        std::function<void(QPDF*, char const*)> fn,
        char const* password, bool empty);

    // Transformations
    void setQPDFOptions(QPDF& pdf);
    void handlePageSpecs(
        QPDF& pdf, bool& warnings,
        std::vector<std::shared_ptr<QPDF>>& page_heap);
    bool shouldRemoveUnreferencedResources(QPDF& pdf);
    void handleRotations(QPDF& pdf);
    void getUOPagenos(UnderOverlay& uo,
                      std::map<int, std::vector<int> >& pagenos);
    void handleUnderOverlay(QPDF& pdf);
    void doUnderOverlayForPage(
        QPDF& pdf,
        UnderOverlay& uo,
        std::map<int, std::vector<int> >& pagenos,
        size_t page_idx,
        std::map<int, QPDFObjectHandle>& fo,
        std::vector<QPDFPageObjectHelper>& pages,
        QPDFPageObjectHelper& dest_page,
        bool before);
    void validateUnderOverlay(QPDF& pdf, UnderOverlay* uo);
    void handleTransformations(QPDF& pdf);
    void addAttachments(QPDF& pdf);
    void copyAttachments(QPDF& pdf);

    // Inspection
    void doInspection(QPDF& pdf);
    void doCheck(QPDF& pdf);
    void showEncryption(QPDF& pdf);
    void doShowObj(QPDF& pdf);
    void doShowPages(QPDF& pdf);
    void doListAttachments(QPDF& pdf);
    void doShowAttachment(QPDF& pdf);

    // Output generation
    void doSplitPages(QPDF& pdf, bool& warnings);
    void setWriterOptions(QPDF& pdf, QPDFWriter& w);
    void setEncryptionOptions(QPDF&, QPDFWriter&);
    void maybeFixWritePassword(int R, std::string& password);
    void writeOutfile(QPDF& pdf);

    // JSON
    void doJSON(QPDF& pdf);
    std::set<QPDFObjGen> getWantedJSONObjects();
    void doJSONObjects(QPDF& pdf, JSON& j);
    void doJSONObjectinfo(QPDF& pdf, JSON& j);
    void doJSONPages(QPDF& pdf, JSON& j);
    void doJSONPageLabels(QPDF& pdf, JSON& j);
    void doJSONOutlines(QPDF& pdf, JSON& j);
    void doJSONAcroform(QPDF& pdf, JSON& j);
    void doJSONEncrypt(QPDF& pdf, JSON& j);
    void doJSONAttachments(QPDF& pdf, JSON& j);

    enum remove_unref_e { re_auto, re_yes, re_no };

    class Members
    {
        friend class QPDFJob;

      public:
        QPDF_DLL
        ~Members() = default;

      private:
        Members();
        Members(Members const&) = delete;

        std::string message_prefix;
        bool warnings;
        std::ostream* cout;
        std::ostream* cerr;
        unsigned long encryption_status;
        bool verbose;
        std::shared_ptr<char> password;
        bool linearize;
        bool decrypt;
        int split_pages;
        bool progress;
        bool suppress_warnings;
        bool warnings_exit_zero;
        bool copy_encryption;
        std::string encryption_file;
        std::shared_ptr<char> encryption_file_password;
        bool encrypt;
        bool password_is_hex_key;
        bool suppress_password_recovery;
        password_mode_e password_mode;
        bool allow_insecure;
        bool allow_weak_crypto;
        std::string user_password;
        std::string owner_password;
        int keylen;
        bool r2_print;
        bool r2_modify;
        bool r2_extract;
        bool r2_annotate;
        bool r3_accessibility;
        bool r3_extract;
        bool r3_assemble;
        bool r3_annotate_and_form;
        bool r3_form_filling;
        bool r3_modify_other;
        qpdf_r3_print_e r3_print;
        bool force_V4;
        bool force_R5;
        bool cleartext_metadata;
        bool use_aes;
        bool stream_data_set;
        qpdf_stream_data_e stream_data_mode;
        bool compress_streams;
        bool compress_streams_set;
        bool recompress_flate;
        bool recompress_flate_set;
        int compression_level;
        qpdf_stream_decode_level_e decode_level;
        bool decode_level_set;
        bool normalize_set;
        bool normalize;
        bool suppress_recovery;
        bool object_stream_set;
        qpdf_object_stream_e object_stream_mode;
        bool ignore_xref_streams;
        bool qdf_mode;
        bool preserve_unreferenced_objects;
        remove_unref_e remove_unreferenced_page_resources;
        bool keep_files_open;
        bool keep_files_open_set;
        size_t keep_files_open_threshold;
        bool newline_before_endstream;
        std::string linearize_pass1;
        bool coalesce_contents;
        bool flatten_annotations;
        int flatten_annotations_required;
        int flatten_annotations_forbidden;
        bool generate_appearances;
        std::string min_version;
        std::string force_version;
        bool show_npages;
        bool deterministic_id;
        bool static_id;
        bool static_aes_iv;
        bool suppress_original_object_id;
        bool show_encryption;
        bool show_encryption_key;
        bool check_linearization;
        bool show_linearization;
        bool show_xref;
        bool show_trailer;
        int show_obj;
        int show_gen;
        bool show_raw_stream_data;
        bool show_filtered_stream_data;
        bool show_pages;
        bool show_page_images;
        size_t collate;
        bool flatten_rotation;
        bool list_attachments;
        std::string attachment_to_show;
        std::list<std::string> attachments_to_remove;
        std::list<AddAttachment> attachments_to_add;
        std::list<CopyAttachmentFrom> attachments_to_copy;
        bool json;
        std::set<std::string> json_keys;
        std::set<std::string> json_objects;
        bool check;
        bool optimize_images;
        bool externalize_inline_images;
        bool keep_inline_images;
        bool remove_page_labels;
        size_t oi_min_width;
        size_t oi_min_height;
        size_t oi_min_area;
        size_t ii_min_bytes;
        UnderOverlay underlay;
        UnderOverlay overlay;
        UnderOverlay* under_overlay;
        std::vector<PageSpec> page_specs;
        std::map<std::string, RotationSpec> rotations;
        bool require_outfile;
        bool replace_input;
        bool check_is_encrypted;
        bool check_requires_password;
        std::shared_ptr<char> infilename;
        std::shared_ptr<char> outfilename;
    };
    std::shared_ptr<Members> m;
};

#endif // QPDFOBJECT_HH
