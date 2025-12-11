/*
 * Copyright (C) 2024-2025 Zexshia
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <AZenith.h>
#include <sys/system_properties.h>

/***********************************************************************************
 * Function Name      : print_help
 * Inputs             : None
 * Returns            : None
 * Description        : Prints all available AZenith Daemon CLI commands to stdout.
 *                      Displays usage instructions for running the daemon,
 *                      selecting profiles, and sending log messages.
 ***********************************************************************************/
void print_help() {
    printf("AZenith Daemon CLI by @Zexshia\n");
    printf("Usage:\n");
    printf("  sys.azenith-service --run\n");
    printf("      Start AZenith daemon\n\n");

    printf("  sys.azenith-service --profile <1|2|3>\n");
    printf("      Apply AZenith Profile manually\n");
    printf("      1 = Performance\n");
    printf("      2 = Balanced\n");
    printf("      3 = Eco Mode\n\n");

    printf("  sys.azenith-service --log <TAG> <LEVEL> <MESSAGE>\n");
    printf("      Write log through AZenith logging service\n");
    printf("      Usage: --log <TAG> <LEVEL> <MESSAGE>\n");
    printf("      Levels: 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR, 4=FATAL\n");
}

/***********************************************************************************
 * Function Name      : handle_profile
 * Inputs             : argc - number of CLI arguments
 *                      argv - array of CLI argument strings
 * Returns            : int - 0 on success, non-zero on failure
 * Description        : Handles manual profile selection. Validates that Auto Mode
 *                      is disabled, reads the requested profile (1/2/3), logs the
 *                      action, sends a toast message, and executes the profiler.
 *                      Profiles:
 *                          1 = Performance
 *                          2 = Balanced
 *                          3 = Eco Mode
 ***********************************************************************************/
int handle_profile(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Missing profile number.\n");
        return 1;
    }

    char ai_state[PROP_VALUE_MAX] = {0};
    __system_property_get("persist.sys.azenithconf.AIenabled", ai_state);

    if (!strcmp(ai_state, "1")) {
        fprintf(stderr, "ERROR: Auto mode enabled. Manual profile blocked.\n");
        return 1;
    }

    const char* profile = argv[2];

    if (!strcmp(profile, "1")) {
        log_zenith(LOG_INFO, "Applying Performance Profile via execute");
        toast("Applying Performance Profile");
        run_profiler(PERFORMANCE_PROFILE);
        printf("Applying Performance Profile\n");        
    } else if (!strcmp(profile, "2")) {
        log_zenith(LOG_INFO, "Applying Balanced Profile via execute");
        toast("Applying Balanced Profile");
        run_profiler(BALANCED_PROFILE);
        printf("Applying Balanced Profile\n");
    } else if (!strcmp(profile, "3")) {
        log_zenith(LOG_INFO, "Applying Eco Mode via execute");
        toast("Applying Eco Mode");
        run_profiler(ECO_MODE);
        printf("Applying Eco Mode\n");
    } else {
        fprintf(stderr, "Invalid profiles.\n");
        return 1;
    }

    return 0;
}

/***********************************************************************************
 * Function Name      : handle_log
 * Inputs             : argc - number of CLI arguments
 *                      argv - array of CLI argument strings
 * Returns            : int - 0 on success, non-zero on failure
 * Description        : Handles the --log command. Validates log level (0..4),
 *                      concatenates the message arguments into a single string,
 *                      and forwards the formatted log entry to the external log
 *                      handler.
 *                      Log Levels:
 *                          0 = DEBUG
 *                          1 = INFO
 *                          2 = WARN
 *                          3 = ERROR
 *                          4 = FATAL
 ***********************************************************************************/
int handle_log(int argc, char** argv) {
    if (argc < 5) {
        fprintf(stderr, "Usage: --log <TAG> <LEVEL> <MESSAGE>\n");
        fprintf(stderr, "Levels: 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR, 4=FATAL\n");
        return 1;
    }

    int level = atoi(argv[3]);
    if (level < LOG_DEBUG || level > LOG_FATAL) {
        fprintf(stderr, "Invalid log level (0..4)\n");
        return 1;
    }

    char message[1024] = {0};
    for (int i = 4; i < argc; i++) {
        strcat(message, argv[i]);
        if (i != argc - 1)
            strcat(message, " ");
    }

    external_log(level, argv[2], message);
    return 0;
}
