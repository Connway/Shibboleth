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
		};
	}

	test(input)
	{
		console.log(input); console.log(input.text); this.state.player_id = Number.parseInt(input.text);
	}

	render()
	{
		const pid_label = React.createElement("label", { for: "player_id_input" }, "Player ID: ");

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

		const pid_container = React.createElement("div", { id: "pid" }, pid_label, pid_input);

		const input_type_label = React.createElement("label", { for: "input_type" }, "Create Inputs: ");
		const input_keyboard_label = React.createElement("label", { for: "create_keyboard" }, "Keyboard: ");
		const input_mouse_label = React.createElement("label", { for: "create_mouse" }, "Mouse: ");

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
		const input_container = React.createElement("div", null, input_type_label, input_type_container, input_button);


		//const form = React.createElement("form", { onSubmit: () => this.submit() }, label, text_input);
		//const container = React.createElement("div", null, pid_container);
		const container = React.createElement("div", null, pid_container, input_container);

		return container;
	}

	createInputs()
	{
		const xhr = new XMLHttpRequest();
		xhr.open("PUT", window.location.href);

		xhr.setRequestHeader("Content-Type", "application/json");

		xhr.onreadystatechange = function ()
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
}

const domContainer = document.querySelector('#index');
ReactDOM.render(React.createElement(InputPage), domContainer);
