'use strict';

class InputPage extends React.Component
{
	constructor(props)
	{
		super(props);

		this.state =
		{
			create_keyboard: false,
			create_mouse: false,
			player_id: -1,
			active_inputs: "None",
			keyboard_input: "",
			mouse_input: [],
		};
	}

	test(input)
	{
		console.log(input); console.log(input.text); this.state.player_id = Number.parseInt(input.text);
	}

	render()
	{
		const active_inputs_container = this.createActiveInputIDGUI();
		const pid_container = this.createPIDGUI();
		const input_container = this.createInputsGUI();
		const input_push_container = this.createPushInputGUI();

		//const form = React.createElement("form", { onSubmit: () => this.submit() }, label, text_input);
		//const container = React.createElement("div", null, pid_container);
		const container = React.createElement("div", null, active_inputs_container, pid_container, input_container, input_push_container);

		return container;
	}

	createActiveInputIDGUI()
	{
		const active_inputs_text = React.createElement("p", null, "Active Input IDs: " + this.state.active_inputs);
		const active_inputs_newline = React.createElement("p", null, React.createElement("br"));
		const active_inputs_button = React.createElement(
			"button",
			{
				onClick: (event) => this.retrieveInputs()
			},
			"Poll Active Inputs"
		);

		return React.createElement("div", { id: "active_inputs" }, active_inputs_text, active_inputs_button, active_inputs_newline);
	}

	createPIDGUI() {
		const pid_label = React.createElement("label", { htmlFor: "player_id_input" }, "Player ID: ");
		const pid_input = React.createElement(
			"input",
			{
				type: "number",
				id: "player_id_input",
				min: -1,
				placeholder: -1,
				onChange: (event) => this.state.player_id = event.target.value
			}
		);

		return React.createElement("div", { id: "pid" }, pid_label, pid_input);
	}

	createInputsGUI()
	{
		const input_newline = React.createElement("p", null, React.createElement("br"));
		const input_type_label = React.createElement("label", { htmlFor: "input_type" }, "Create Inputs: ");
		const input_keyboard_label = React.createElement("label", { htmlFor: "create_keyboard" }, "Keyboard: ");
		const input_mouse_label = React.createElement("label", { htmlFor: "create_mouse" }, "Mouse: ");

		const input_type_keyboard = React.createElement(
			"input",
			{
				type: "checkbox",
				onChange: (event) => this.state.create_keyboard = event.target.checked,
				id: "create_keyboard"
			}
		);

		const input_type_mouse = React.createElement(
			"input",
			{
				type: "checkbox",
				onChange: (event) => this.state.create_mouse = event.target.checked,
				id: "create_mouse"
			}
		);

		const input_button = React.createElement(
			"button",
			{
				onClick: (event) => this.createInputs()
			},
			"Create"
		);

		const input_type_container = React.createElement("div", { id: "input_type" }, input_keyboard_label, input_type_keyboard, input_mouse_label, input_type_mouse);
		return React.createElement("div", null, input_newline, input_type_label, input_type_container, input_button);
	}

	createPushInputGUI()
	{
		const input_newline = React.createElement("p", null, React.createElement("br"));
		const keyboard_input_label = React.createElement("label", { htmlFor: "keyboard_input" }, "Keyboard Input: ");
		const mouse_input_label = React.createElement("label", { htmlFor: "mouse_input" }, "Mouse Input: ");

		const keyboard_input = React.createElement(
			"input",
			{
				type: "text",
				onChange: (event) => this.state.keyboard_input = event.target.value,
				id: "keyboard_input"
			}
		);

		const push_inputs_button = React.createElement(
			"button",
			{
				onClick: (event) => this.pushInputs()
			},
			"Push Inputs"
		);


		//const left_click_button = React.createElement(
		//	"button",
		//	{
		//		onClick: (event) => this.createInputs()
		//	},
		//	"Left Click"
		//);

		//const mouse_input_container = React.createElement("div", { id: "mouse_input" });

		const keyboard_container = React.createElement("div", { id: "keyboard_input" }, keyboard_input);
		return React.createElement("div", null, input_newline, keyboard_input_label, keyboard_container/*, mouse_input_container*/, React.createElement("br"), push_inputs_button);
	}

	retrieveInputs()
	{
		const xhr = new XMLHttpRequest();
		const this_ref = this;

		xhr.open("OPTIONS", window.location.href);
		xhr.setRequestHeader("Content-Type", "application/json");

		xhr.onreadystatechange = function()
		{
			if (xhr.readyState === XMLHttpRequest.DONE) {
				// The request has been completed successfully
				if (xhr.status === 0 || (xhr.status >= 200 && xhr.status < 400)) {
					const player_devices = JSON.parse(xhr.responseText);
					var result = "";

					if (player_devices.length <= 0) {
						this_ref.state.active_inputs = "None";
						return;
					}

					for (const key in player_devices) {
						result += '\n' + key;
					}

					this_ref.state.active_inputs = result;
					this_ref.setState(this_ref.state);

				} else {
					// Oh no! There has been an error with the request!
				}
			}
		};

		const data = `{
		  "request_type": "player_devices",
		}`;

		xhr.send(data);
	}

	createInputs()
	{
		const xhr = new XMLHttpRequest();
		xhr.open("PUT", window.location.href);

		xhr.setRequestHeader("Content-Type", "application/json");

		xhr.onreadystatechange = function()
		{
			if (xhr.readyState === XMLHttpRequest.DONE) {
				if (xhr.status === 0 || (xhr.status >= 200 && xhr.status < 400)) {
					// The request has been completed successfully
					console.log(xhr.responseText);
				} else {
					// Oh no! There has been an error with the request!
				}
			}
		};

		const data = `{
		  "player_id": ${this.state.player_id},
		  "create_keyboard": ${this.state.create_keyboard},
		  "create_mouse": ${this.state.create_mouse}
		}`;

		xhr.send(data);
	}

	pushInputs()
	{
		const xhr = new XMLHttpRequest();
		xhr.open("POST", window.location.href);

		xhr.setRequestHeader("Content-Type", "application/json");

		xhr.onreadystatechange = function () {
			if (xhr.readyState === XMLHttpRequest.DONE) {
				if (xhr.status === 0 || (xhr.status >= 200 && xhr.status < 400)) {
					// The request has been completed successfully
					console.log(xhr.responseText);
				} else {
					// Oh no! There has been an error with the request!
				}
			}
		};

		var data = `{
			"player_id": ${this.state.player_id},
			"inputs": [`;

		// Could probably replace this with a call to this.state.keyboard_input.match() instead.
		var prev = 0;

		for (var i = 0; i < this.state.keyboard_input.length; ++i) {
			if (this.state.keyboard_input.charAt(i) == ",") {
				var key_code = this.state.keyboard_input.substring(prev, i);
				prev = i + 1;

				data += `{ "key_code": "${key_code}", "value": 1.0 },`;
			}
		}

		if (prev < this.state.keyboard_input.length) {
			var key_code = this.state.keyboard_input.substring(prev, this.state.keyboard_input.length);
			data += `{ "key_code": "${key_code}", "value": 1.0 }`;
		}

		if (data.endsWith(",")) {
			data = data.substr(0, data.length - 1);
		}

		for (var mouse_input in this.state.mouse_input) {
		}

		data += "]\n}";

		console.log(data);

		xhr.send(data);
	}
}

const domContainer = document.querySelector('#index');
ReactDOM.render(React.createElement(InputPage), domContainer);
