// NETSPOT - Sidebar Navigation

document.addEventListener("DOMContentLoaded", () => {

    const navButtons = document.querySelectorAll(".nav button");

    navButtons.forEach(button => {

        button.addEventListener("click", () => {

            navButtons.forEach(item => {
                item.classList.remove("active");
            });

            button.classList.add("active");

            const page = button.dataset.page;

            const pageTitle = document.getElementById("pageTitle");

            if (pageTitle) {
                pageTitle.textContent =
                    page === "Dashboard"
                    ? "SYSTEM OVERVIEW"
                    : page.toUpperCase();
            }

            notify(page + " selected");

        });

    });

});
