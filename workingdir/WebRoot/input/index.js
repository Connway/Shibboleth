'use strict';

class LikeButton extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state =
        {
            create_keyboard: false,
            create_mouse: false
        };
    }

    render()
    {
        const pid_label = React.createElement("label", { for: "player_id_input" }, "Player ID: ");
        const pid_input = React.createElement("input", { type: "text", id: "player_id_input",  });
        const pid_container = React.createElement("div", { id: "pid" }, pid_label, pid_input);

        const input_type_label = React.createElement("label", { for: "input_type" }, "Create Inputs: ");
        const input_keyboard_label = React.createElement("label", { for: "create_keyboard" }, "Keyboard: ");
        const input_mouse_label = React.createElement("label", { for: "create_mouse" }, "Keyboard: ");

        const input_type_keyboard = React.createElement(
            "input",
            {
                type: "checkbox",
                onChange: (cbox) => this.state.create_keyboard = cbox.target.checked,
                id: "create_keyboard"
            }
        );

        const input_type_mouse = React.createElement(
            "input",
            {
                type: "checkbox",
                onChange: (cbox) => this.state.create_mouse = cbox.target.checked,
                id: "create_mouse"
            }
        );

        const input_type_container = React.createElement("div", { id: "input_type" }, input_keyboard_label, input_type_keyboard, input_mouse_label, input_type_mouse);
        const input_container = React.createElement("div", null, input_type_label, input_type_container);


        //const form = React.createElement("form", { onSubmit: () => this.submit() }, label, text_input);
        //const container = React.createElement("div", null, pid_container);
        const container = React.createElement("div", null, pid_container, input_container);

        return container;
    }

    submit()
    {
    }
}

const domContainer = document.querySelector('#index');
ReactDOM.render(React.createElement(LikeButton), domContainer);
