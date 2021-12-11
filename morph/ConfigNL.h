/*
 * A wrapper around nlohmann JSON code for saving and retrieving parameters
 *
 * Fixme: All I want in addition to what json offers is ability to open a file, e.g. nlohmann::json j(file_path);
 * Could get this by extending?
 */
#pragma once

#include <morph/nlohmann/json.hpp>
#include <list>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#ifndef __WIN__
# include <morph/Process.h>
#endif

namespace morph {
#ifndef __WIN__
    //! Callbacks class extends ProcessCallbacks
    class ConfigProcessCallbacks : public ProcessCallbacks
    {
    public:
        ConfigProcessCallbacks (ProcessData* p) { this->parent = p; }
        void startedSignal (std::string msg) {}
        void errorSignal (int err) { this->parent->setErrorNum (err); }
        void processFinishedSignal (std::string msg) { this->parent->setProcessFinishedMsg (msg); }
        void readyReadStandardOutputSignal (void) { this->parent->setStdOutReady (true); }
        void readyReadStandardErrorSignal (void) { this->parent->setStdErrReady (true); }
    private:
        ProcessData* parent;
    };
#endif
    /*!
     * A configuration file class to help read simulation parameters from a JSON file.
     *
     * This reads a config file which should be arranged as a JSON file. The format is fairly
     * free-form; getters and setters are used to access the parameters stored in the config file.
     *
     * This class also provides code for updating the JSON config and writing out the updated config
     * into the log directory to make a record of the parameters used to generate a set of
     * simulation data.
     */
    class ConfigNL
    {
    public:
        //! Default constructor, when config should be a class member. Call init() before use.
        ConfigNL(){}

        //! Constructor which takes the path to the file that contains the JSON.
        ConfigNL (const std::string& configfile) { this->init (configfile); }

        //! Perform config file initialization.
        void init (const std::string& configfile)
        {
            this->thefile = configfile;
            std::stringstream ess;
            // Test for existence of the JSON file.
            std::ifstream jsonfile_test;
            jsonfile_test.open (configfile, std::ios::in);
            if (jsonfile_test.is_open()) {
                // File exists, should parse it
                jsonfile_test.close();
                // Parse the JSON
                std::ifstream jsonfile (configfile, std::ifstream::binary);
                jsonfile >> this->root;
                // JSON is open and parsed
            } // else We are creating a new Config, with no pre-existing content
        }

#ifndef __WIN__
        /*!
         * Launch git sub-processes to determine info about the current
         * repository. Intended for use with code that will save a Json formatted log of
         * a simulation run.
         *
         * \a codedir The name of the directory in which significant code is located. If
         * git status detects changes in this directory, then information to this effect
         * will be inserted into this->root.
         */
        void insertGitInfo (const std::string& codedir)
        {
            ProcessData pD;
            ConfigProcessCallbacks cb(&pD);
            Process p;
            std::string command ("/usr/bin/git");

            std::list<std::string> args1;
            args1.push_back ("git");
            args1.push_back ("rev-parse");
            args1.push_back ("HEAD");

            try {
                p.setCallbacks (&cb);
                p.start (command, args1);
                p.probeProcess ();
                if (!p.waitForStarted()) {
                    throw std::runtime_error ("Process failed to start");
                }
                while (p.running() == true) {
                    p.probeProcess();
                }

                std::stringstream theOutput;
                theOutput << p.readAllStandardOutput();
                std::string line = "";
                int nlines = 0;
                while (getline (theOutput, line, '\n')) {
                    std::cout << "Current git HEAD: " << line << std::endl;
                    if (nlines++ > 0) {
                        break;
                    }
                    this->root["git_head"] = line; // Should be one line only
                }

            } catch (const std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                this->root["git_head"] = "unknown";
            }

            // Reset Process with arg true to keep callbacks
            p.reset (true);

            std::list<std::string> args2;
            args2.push_back ("git");
            args2.push_back ("status");

            try {
                p.start (command, args2);
                p.probeProcess ();
                if (!p.waitForStarted()) {
                    throw std::runtime_error ("Process failed to start");
                }
                while (p.running() == true) {
                    p.probeProcess();
                }

                std::stringstream theOutput;
                theOutput << p.readAllStandardOutput();
                std::string line = "";
                bool lm = false;
                bool ut = false;
                while (getline (theOutput, line, '\n')) {
                    if (line.find("modified:") != std::string::npos) {
                        if (line.find(codedir) != std::string::npos) {
                            if (!lm) {
                                this->root["git_modified_sim"] = true;
                                std::cout << "Repository has local modifications in " << codedir << " dir" << std::endl;
                            }
                            lm = true;
                        }
                    }
                    if (line.find("Untracked files:") != std::string::npos) {
                        if (line.find(codedir) != std::string::npos) {
                            if (!ut) {
                                this->root["git_untracked_sim"] = true;
                                std::cout << "Repository has untracked files present in " << codedir << " dir" << std::endl;
                            }
                            ut = true;
                        }
                    }
                }

            } catch (const std::exception& e) {
                std::stringstream ee;
                ee << "Exception: " << e.what();
                this->emsg = ee.str();
                this->root["git_status"] = "unknown";
            }

            // Reset for third call
            p.reset (true);

            // This gets the git branch name
            std::list<std::string> args3;
            args3.push_back ("git");
            args3.push_back ("rev-parse");
            args3.push_back ("--abbrev-ref");
            args3.push_back ("HEAD");

            try {
                p.start (command, args3);
                p.probeProcess ();
                if (!p.waitForStarted()) {
                    throw std::runtime_error ("Process failed to start");
                }
                while (p.running() == true) {
                    p.probeProcess();
                }

                std::stringstream theOutput;
                theOutput << p.readAllStandardOutput();
                std::string line = "";
                int nlines = 0;
                while (getline (theOutput, line, '\n')) {
                    std::cout << "Current git branch: " << line << std::endl;
                    if (nlines++ > 0) {
                        break;
                    }
                    this->root["git_branch"] = line; // Should be one line only
                }

            } catch (const std::exception& e) {
                std::stringstream ee;
                ee << "Exception: " << e.what();
                this->emsg = ee.str();
                this->root["git_branch"] = "unknown";
            }
        }
#endif // __WIN__

        void write() const
        {
            std::ofstream configout;
            configout.open (this->thefile.c_str(), std::ios::out|std::ios::trunc);
            if (configout.is_open()) {
                configout << this->root;
                configout.close();
            } else {
                ////////////this->emsg = std::string("Failed to open file '") + this->thefile + std::string("' for writing");
            }
        }

        //! Write out the JSON to a different file.
        void write (std::string& outfile)
        {
            std::ofstream configout;
            configout.open (outfile.c_str(), std::ios::out|std::ios::trunc);
            if (configout.is_open()) {
                configout << this->root;
                configout.close();
            } else {
                this->emsg = "Failed to open file '" + outfile + "' for writing";
            }
        }

        //! Output the config as a string of text
        std::string str() const
        {
            std::stringstream ss;
            ss << this->root;
            return ss.str();
        }

        // Wrappers around gets
        template <typename T>
        T get (const std::string& thing, T defaultval) const
        {
            return this->root.contains(thing) ? this->root[thing].get<T>() : defaultval;
        }

        bool getBool (const std::string& thing, bool defaultval) const
        {
            return this->get<bool> (thing, defaultval);
        }
        int getInt (const std::string& thing, int defaultval) const
        {
            return this->get<int> (thing, defaultval);
        }
        unsigned int getUInt (const std::string& thing, unsigned int defaultval) const
        {
            return this->get<unsigned int> (thing, defaultval);
        }
        float getFloat (const std::string& thing, float defaultval) const
        {
            return this->get<float> (thing, defaultval);
        }
        double getDouble (const std::string& thing, double defaultval) const
        {
            return this->get<double> (thing, defaultval);
        }
        std::string getString (const std::string& thing, const std::string& defaultval) const
        {
            return this->get<std::string> (thing, defaultval);
        }
        auto getArray (const std::string& arrayname) const
        {
            return this->root[arrayname];
        }
        // getValue - just use templated get

        // Setters
        template <typename T>
        void set (const std::string& thing, T value) { this->root[thing] = value; }
        template <typename T>
        void setArray (const std::string& thing, const std::vector<T>& values) { this->root[thing] = values; }

        //! Any error message is here.
        std::string emsg = "";

        // The root object which is set up in the constructor
        nlohmann::json root;

        // The file that holds the JSON
        std::string thefile = "";
#if 0
    private:
        //! Private, only intended for getters/setters use. Client code can directly test Config::ready.
        void checkready (const std::string& caller) const
        {
            if (!this->ready) {
                std::stringstream ee;
                ee << "Config not ready. Error was: " << this->emsg << ". Can't " << caller << ". Check your JSON.";
                throw std::runtime_error (ee.str());
            }
        }
#endif
    };
} // namespace
