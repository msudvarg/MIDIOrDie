
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "tensorflow/cc/saved_model/loader.h"
#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/cc/client/client_session.h"
#include "tensorflow/core/util/command_line_flags.h"
#include "tensorflow/core/framework/tensor_slice.h"

using namespace std;
using tensorflow::int32;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::Tensor;
using tensorflow::tstring;
using tensorflow::SavedModelBundle;
using tensorflow::SessionOptions;
using tensorflow::RunOptions;
using tensorflow::Scope;
using tensorflow::ClientSession;

class ModelLoader{
	private:
		SavedModelBundle bundle;
		SessionOptions session_options;
		RunOptions run_options;
        Tensor calib;
		void make_prediction(std::vector<Tensor> &inputs, std::vector<float> &out_pred);
	public:
		ModelLoader(string);
		void predict(std::vector<float> audio, std::vector<float> &out_pred);
		void set_calibrations(std::vector<float> calib_vector);
};