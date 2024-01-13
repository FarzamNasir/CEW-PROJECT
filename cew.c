#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

// Callback function to handle received data
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    char *data = (char *)userp;

    // Append received data to the user buffer
    memcpy(data, contents, realsize);

    // Add null terminator to properly terminate the string
    data[realsize] = '\0';

    return realsize;
}

// Function to get and format the current time
void getCurrentFormattedTime(char formatted_time[50], size_t size) {
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    strftime(formatted_time, size, "%d/%b/%Y\nTime: %H:%M:%S", local_time);
}


   	int Handling() {
   	
    time_t now;
    struct tm *local_time;

    // Get current time
    now = time(NULL);
    local_time = localtime(&now); // Obtain local time components
    
    char formatted_time[50];
    getCurrentFormattedTime(formatted_time, sizeof(formatted_time));

    // Check if it's Monday
    if (local_time->tm_wday == 1) {
    
    FILE *Report_file = fopen("Report_Handling.txt", "r+");
    if (Report_file == NULL) {
        perror("Error opening Report_Handling.txt");
        return 1;
    }

    int ch;
    // Process the first line
    while ((ch = fgetc(Report_file)) != EOF && ch != '\n') {
        if (ch == '0') {
        // Delete the contents of "Temp.json"
            FILE *TempFile = fopen("Temp.json", "w");
            if (TempFile == NULL) {
                perror("Error opening Temp.json");
                fclose(Report_file);
                return 1;
            }
            fclose(TempFile);
            fseek(Report_file, -1, SEEK_CUR);
            fputc('1', Report_file);
            break;
        }
    }

    // Move to the beginning of the second line
    while ((ch = fgetc(Report_file)) != EOF && ch != '\n');

    // Process the second line
    while ((ch = fgetc(Report_file)) != EOF && ch != '\n') {
        if (ch == '1') {
            fseek(Report_file, -1, SEEK_CUR);
            fputc('0', Report_file);
            break;
        }
    }

    fclose(Report_file);
    }if (local_time->tm_wday == 6){ //check's sunday
    
    
    FILE *Report_file = fopen("Report_Handling.txt", "r+");
    if (Report_file == NULL) {
        perror("Error opening Report_Handling.txt");
        return 1;
    }

    int ch;
    // Process the first line
    while ((ch = fgetc(Report_file)) != EOF && ch != '\n') {
        if (ch == '1') {
            fseek(Report_file, -1, SEEK_CUR);
            fputc('0', Report_file);
            break;
        }
    }

    // Move to the beginning of the second line
    while ((ch = fgetc(Report_file)) != EOF && ch != '\n');

    // Process the second line
    while ((ch = fgetc(Report_file)) != EOF && ch != '\n') {
        if (ch == '0') {
        fseek(Report_file, -1, SEEK_CUR);
        fputc('1', Report_file);
        
            FILE *temp_file_read = fopen("Temp.json", "r");
	if (temp_file_read != NULL) {
	    double sum = 0.0;
	    int count = 0;
	    double temperature;

	    // Read temperatures from the file and calculate sum
	    while (fscanf(temp_file_read, "%lf\n", &temperature) == 1) {
		sum += temperature;
		count++;
	    }

	    fclose(temp_file_read);

	    if (count > 0) {// 'w' use krrhe cz hr baar next week mai uski report generate krni
		double mean = sum / count;
		//printf("Sum of temperatures: %.2f\n", sum);
		//printf("Mean temperature: %.2f\n", mean);
		  FILE *Weekly_Report_file = fopen("Weekly_Weather_Report.txt", "w"); // Add .txt extension
			if (Weekly_Report_file != NULL) {
			    fprintf(Weekly_Report_file, "Report Generated on: %s\n", formatted_time);
			    fprintf(Weekly_Report_file, "Mean temperature: %.2f\n", mean);
			    fclose(Weekly_Report_file);
			} else {
			    fprintf(stderr, "Failed to open Weekly_Weather_Report.txt for writing.\n");
			}


		//exit after report
		return 0;
	    } else {
		printf("No temperature values found in the file.\n");
	    }
} else {
    fprintf(stderr, "Failed to open Temp.json for reading.\n");
}
        
        
            //fseek(Report_file, -1, SEEK_CUR);
            //fputc('1', Report_file);
            break;
        }else {
        printf("Report has been already generated. \nCheck the Weekly Report file on your desktop.\n");
        break;
        }
    }

    fclose(Report_file);
    
    
    }
}
   

int main() {
    CURL *curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        const char *url = "https://api.open-meteo.com/v1/forecast?latitude=24.923551&longitude=67.133765&current=temperature_2m,relative_humidity_2m,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m&timezone=auto&forecast_days=1";
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the callback function to handle the received data
        char response_data[4096]; // Adjust the buffer size accordingly
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);

        // Perform the HTTP request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }

        // Save raw data to "raw_data.json"
        FILE *raw_data_file = fopen("raw_data.json", "a"); // Use "a" for append mode
        if (raw_data_file != NULL) {
            fprintf(raw_data_file, "%s\n\n\n", response_data);
            fclose(raw_data_file);
        } else {
            fprintf(stderr, "Failed to open raw_data.json for writing.\n");
        }

        // Parse JSON data
        json_t *root;
        json_error_t error;
        root = json_loads(response_data, 0, &error);
        if (!root) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }

        // Access temperature, humidity, and wind_speed values
        json_t *temperature = json_object_get(json_object_get(root, "current"), "temperature_2m");
        //json_t *humidity = json_object_get(json_object_get(root, "current"), "relative_humidity_2m");
        json_t *wind_speed = json_object_get(json_object_get(root, "current"), "wind_speed_10m");
        json_t *wind_direction = json_object_get(json_object_get(root, "current"), "wind_direction_10m");
        json_t *surface_pressure = json_object_get(json_object_get(root, "current"), "surface_pressure");
        json_t *sealevel_pressure = json_object_get(json_object_get(root, "current"), "pressure_msl");
        
        //time_t current_time = time(NULL);
        //struct tm *local_time = localtime(&current_time);
        //char formatted_time[50]; // Allocate space for the formatted time
//strftime(formatted_time, sizeof(formatted_time), "%d/%b/%Y\n"
//"Time: %H:%M:%S", local_time);
	char formatted_time[50];
    	getCurrentFormattedTime(formatted_time, sizeof(formatted_time));
    	
	Handling();
	
	FILE *temp_file = fopen("Temp.json", "a");
    	if (temp_file != NULL) {
    		//fseek(temp_file, 0, SEEK_END);
        	fprintf(temp_file, "%.2f\n", json_real_value(temperature));
        	fclose(temp_file);	
    	} else {
        	fprintf(stderr, "Failed to open Temp.txt for writing.\n");
    	}
    	
    	
    	
    	
 

    	
    	
        
        // Save processed data to "processed_data"
    	FILE *processed_data_file = fopen("processed_data.json", "a"); // Use "a" for append mode
        if (processed_data_file != NULL) {
            fprintf(processed_data_file, "- - - - - - - - - - - - - Processed Data - - - - - - - - - - - - -\n");
            fprintf(processed_data_file,"Report Generated on: %s\n", formatted_time);
            fprintf(processed_data_file, "Temperature: %.2f°C\n", json_real_value(temperature));
            //fprintf(processed_data_file, "Humidity: %.2f%%\n", json_real_value(humidity));
            fprintf(processed_data_file, "Wind Speed(10m): %.2f km/h\n", json_real_value(wind_speed));
            fprintf(processed_data_file,"Wind Direction(10m): %.2f °\n", json_real_value(wind_direction));
            fprintf(processed_data_file,"Surface Pressure: %.2f hPa\n", json_real_value(surface_pressure));
            fprintf(processed_data_file,"Sealevel Pressure: %.2f hPa\n\n", json_real_value(sealevel_pressure));
            fclose(processed_data_file);
        } else {
            fprintf(stderr, "Failed to open processed_data.txt for writing.\n");
        }
        
   
        // Print the processed values
   	printf("---------------------------------------------------------------\n");
    	printf("                     Weather Report                            \n");
    	printf("---------------------------------------------------------------\n");
	printf("Report Generated on: %s\n", formatted_time);
	printf("Temperature: %.2f°C\n", json_real_value(temperature));
        //printf("Humidity: %.2f%%\n", json_real_value(humidity));
        printf("Wind Speed(10m): %.2f km/h\n", json_real_value(wind_speed));
        printf("Wind Direction(10m): %.2f °\n", json_real_value(wind_direction));
        printf("Surface Pressure: %.2f hPa\n", json_real_value(surface_pressure));
        printf("Sealevel Pressure: %.2f hPa\n", json_real_value(sealevel_pressure));

		// Inside your main function after printing processed data

	// Define thresholds for temperature, humidity, and wind speed
	const float TEMP_THRESHOLD = 10.0; // Example threshold for temperature
	//const float HUMIDITY_THRESHOLD = 70.0; // Example threshold for humidity
	const float WIND_SPEED_THRESHOLD = 10.0; // Example threshold for wind speed

	// Check for anomalies
if (json_real_value(temperature) > TEMP_THRESHOLD) {
    printf("Alert: High Temperature Detected!\n");

    // Send notification
    char notify_command[256];
    snprintf(notify_command, sizeof(notify_command), "notify-send 'High Temperature Alert' 'Temperature is above threshold: %.2f°C'", json_real_value(temperature));
    system(notify_command);

    // Send email with corrected authentication, content, and enhanced security
	    char email_command[1024];
	snprintf(email_command, sizeof(email_command),
	    "echo -e 'From: farzamnasir16@gmail.com\nTo:ahmednevermind01@gmail.com\nSubject: High Temperature Alert\n\n Temperature is is above threshold: %.2f°C' | "
	    "curl --ssl-reqd --url 'smtps://smtp.gmail.com:465' "
	    "-u 'farzamnasir16@gmail.com:sgvt ntje kgdm lisw' "  // Added space after the colon
	    "--mail-from 'farzamnasir16@gmail.com' "
	    "--mail-rcpt 'ahmednevermind01@gmail.com' "
	    "--upload-file -", // Remove --upload-file option
	    json_real_value(temperature));
	system(email_command);
}
	

	//if (json_real_value(humidity) > HUMIDITY_THRESHOLD) {
	 //   printf("Alert: High Humidity Detected!\n");
	//}

			


	if (json_real_value(wind_speed) > WIND_SPEED_THRESHOLD) {
	    printf("Alert: High Wind Speed Detected!\n");
		    char notify_command[256];
	    snprintf(notify_command, sizeof(notify_command), "notify-send 'High Wind Speed Alert' 'Wind Speed is above threshold: %.2fkm/h'", json_real_value(wind_speed));
	    system(notify_command);

	    // Send email with corrected authentication, content, and enhanced security
		    char email_command[1024];//
		snprintf(email_command, sizeof(email_command),
		    "echo -e 'From: farzamnasir16@gmail.com\nTo:ahmednevermind01@gmail.com\nSubject: High Wind Speed Alert\n\n Wind Speed is above threshold: %.2fkm/h' | "
		    "curl --ssl-reqd --url 'smtps://smtp.gmail.com:465' "
		    "-u 'farzamnasir16@gmail.com:sgvt ntje kgdm lisw' "  // Added space after the colon
		    "--mail-from 'farzamnasir16@gmail.com' "
		    "--mail-rcpt 'ahmednevermind01@gmail.com' "
		    "--upload-file -", // Remove --upload-file option
		    json_real_value(wind_speed));
		system(email_command);
	}

   

        // Cleanup
        json_decref(root);
        curl_easy_cleanup(curl);
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return 0;
}
