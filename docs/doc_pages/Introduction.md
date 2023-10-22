@page Introduction Introduction

# Software Engineering First Week Introduction

Welcome to the Software Engineering Project Week. This markdown file provides a step-by-step introduction to the ECIQC structure with a mixture of explanations and tasks. The tasks are designed to provide hands-on experience, and you can skip anything if it's too easy for you. Given the different backgrounds and expertise of group members, you are encouraged to help each other throughout the journey.

If you have any questions, please don't hesitate to reach out to Yiming Wei (yiming.wei@dtc.ox.ac.uk) or Dr. Martin Robinson (martin.robinson@dtc.ox.ac.uk).

### Step 1: Ensure You Are a Member of Extensible-Clinical-Imaging-QC-Tool
You should have all received an invitation. If not, email Yiming or Martin.

### Step 2: Git Clone to Your Own Laptop or SABS Laptop
The ECIQC should work for both MacOS and Linux systems. If you're using the Windows system, install Windows Subsystem for Linux (WSL). If your Windows system does not support WSL, contact Martin for an SABS laptop.

### Step 3: Go Through the README File in the Root Directory to Run ECIQC Locally
This should be quite straightforward. The DICOM images and two log files in the `./result` directory indicate successful running. Read through log files to understand what's happening.

### Step 4: Get Familiar with the Codebase.

### Step 5: Get Familiar with DICOM Format
Digital Imaging and Communications in Medicine (DICOM) is the standard for the communication and management of medical imaging information and related data. The ECIQC primarily works on DICOM images. You are encouraged to use Google/ChatGPT to learn more about the DICOM format.

All files in the ./DICOM_Images are in the DICOM format. To play around, use Python/Matlab and relevant packages to read these images, understand how to get access to metadata, and try to visualise them.

### Step 6: Understand How Configuration Files Are Written
Working through this part might seem challenging initially. Review the tutorial and configuration gallery to grasp the general rules. Last year, we invested a significant amount of time understanding these concepts directly from the source code. Hopefully this year you won't face the same "painful" experience.

Once you feel comfortable, experiment to test your understanding. Create your configuration file and input it into `pipeline_test_main.py`. Customise your image by adjusting the input file route. We found the following example intriguing:
Explore the distinction between OVERWRITE and INSERT. Configuration files with these actions are available in the configuration gallery. However, the exact behaviour of OVERWRITE when the tag doesn't exist and the behaviour of INSERT when the tag already exists remain unclear. It would be valuable if you could uncover these answers through your own experiments.

### Step 7: Polish Documents
After completing step 6, document your findings in the files located in the `./docs` directory. We utilise dOxygen to generate documentation. Explore dOxygen tutorials on various platforms such as webpages and YouTube. Install dOxygen on your local machine. Navigate to `./docs` and execute `doxygen eciqcDoxConfig` to regenerate the documents. Incorporate your insights into `./docs/doc_pages/tutorial.md` and ensure the documentation is up to date.

### Step 8: End-to-End Testing
By now, in step 6, you should understand how to specify your own configuration file and input images when running Python scripts. Step 8 is an extension of this understanding. Here, we aim to verify whether ECIQC behaves as expected with your specified configuration file and input image. You can create lists of configuration files and input images, testing each pair. If all pairs work as expected, that's fantastic. If not, we may need to revisit the parser source code and investigate any abnormal behaviors.

This phase of the project was conceptualised in February after we wrote pipelinetest_main.py, but it was never completed. You might find some old Jupyter notebook files in the `i71-pipeline` directory useful. It's also advisable to incorporate end-to-end testing into the Continuous Integration (CI) process.


### Step 9: Python Plug-in
This year, a primary focus will be on integrating Python plug-ins. In the previous year, we dedicated the entire first week to studying image segmentation and localisation but lacked the opportunity to implement them. If time allows, consider exploring relevant papers and attempting to implement their algorithms. Additionally, contemplate the interface between C++ and Python plug-ins. The Python plug-in should be incorporated immediately after the image-editor in the `Conductor::process_dataset()` method.

That's it! We hope you find this introduction useful, and you will build on your work (steps 8 and 9) after reading this page. Feel free to ask any questions to Yiming or Martin; we are more than happy to help.
