#include "sigextr.h"

ModelLoader::ModelLoader(string path){		
	auto status = tensorflow::LoadSavedModel(session_options, run_options, path, {"serve"},
			&bundle);

	if (status.ok()){
		printf("Model loaded successfully...\n");
	}
	else {
		printf("Error in loading model\n");
	}

}

void ModelLoader::predict(std::vector<float> audio, std::vector<float> calib_vector, std::vector<float> &out_pred){
    Tensor input(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, 80}));
    std::copy_n(audio.begin(), audio.size(), input.flat<float>().data());

    Tensor calib = Tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, 3, 80}));
    std::copy_n(calib_vector.begin(), calib_vector.size(), calib.flat<float>().data());

	std::vector<Tensor> inputs = {input, calib};
	make_prediction(inputs, out_pred);
}

void ModelLoader::make_prediction(std::vector<Tensor> &inputs, std::vector<float> &out_pred){
	const string input_node = "serving_default_main_input:0";
	const string calib_node = "serving_default_calibrations:0";
	std::vector<std::pair<string, Tensor>> inputs_data  = {{input_node, inputs[0]}, {calib_node, inputs[1]}};
	std::vector<string> output_nodes = {{"StatefulPartitionedCall:0"}}; //num_detections

	std::vector<Tensor> predictions;
	auto status = this->bundle.GetSession()->Run(inputs_data, {"StatefulPartitionedCall:0"}, {}, &predictions);
	if (!status.ok()) {
		std::cout << status.ToString() << "\n";
		return;
	}

	auto predicted_notes = predictions[0].flat<float>();
	
	//inflate with predictions
	for (int i=0; i < predicted_notes.size(); i++){
		out_pred.push_back( predicted_notes(i));
	}
}